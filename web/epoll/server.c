//docker run -it -p 1555:1555 -v $(pwd):/src gcc:latest bash
//gcc -O3 -falign-functions=4 -falign-jumps -falign-loops -Wal
//-std=c99 -pedantic
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>


#include <errno.h>
#include <signal.h>
#include <sys/timerfd.h>
#include <sys/signalfd.h>


#include <netinet/in.h>
#include <netinet/tcp.h>
// #include <sys/temporace.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#include <sys/epoll.h>

#define PORT  "1555";
int socket_backlog = SOMAXCONN;

#define MAX_EVENTS  10

#define handle_error_en( en, msg ) \
  do { errno = en; perror ( msg ) ; exit ( EXIT_FAILURE ); } while ( 0 )
#define handle_error( msg ) \
  do { perror ( msg ); exit ( EXIT_FAILURE ); } while ( 0 )
#define return_error_en( en,msg ) \
  do { errno = en; perror ( msg ); return -1; } while ( 0 )
#define return_error( msg ) \
  do { perror ( msg ); return -1; } while ( 0 )


/* --------------------------------------------------------*/
static int
create_and_bind( char *port )
{
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int s, sfd;

  // Get an internet address on ( this host ) port, with these hints
  // Clear hints with zeroes ( as required by getaddrinfo ( ) )

  memset ( &hints, 0, sizeof ( struct addrinfo ) );
  hints.ai_family = AF_INET;  //AF_UNSPEC   /* Return IPv4 and IPv6 choices */
  hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
  hints.ai_flags = AI_PASSIVE;     /* use my IP, Suitable to bind a server onto */

  s = getaddrinfo ( NULL, port, &hints, &result );
  if ( s != 0 )
  {
    fprintf ( stderr, "getaddrinfo: %s\n", gai_strerror ( s ) );
    return -1;
  }

  // For each addrinfo returned, try to open a socket and bind to the
  // associated address; keep going until it works
  for ( rp = result; rp != NULL; rp = rp->ai_next )
  {
    sfd = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol );
    if ( sfd == -1 )
      continue;

        // SO_DEBUG,
          int yes = 1;
      		s=setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
          if ( s != 0 ) {
            fprintf ( stderr, "failed to set SO_REUSEADDR: %d\n", errno );
            // error(1, errno, "failed to set SO_REUSEPORT");
            return -1;
          }


          s = setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(int));
          if ( s != 0 ) {
            fprintf ( stderr, "failed to set SO_REUSEPORT: %d\n", errno );
            // error(1, errno, "failed to set SO_REUSEPORT");
            return -1;
          }
          //example https://github.com/torvalds/linux/blob/master/tools/testing/selftests/net/reuseport_bpf.c
          // https://github.com/masahir0y/linux-yamada/blob/master/tools/testing/selftests/net/reuseport_bpf_numa.c
          // see example to bound to cpu tools/testing/selftests/net/reuseport_bpf_cpu.c
          setsockopt(sfd, SOL_SOCKET, SO_ATTACH_REUSEPORT_EBPF, &yes, sizeof(int));
      		setsockopt(sfd, IPPROTO_TCP, TCP_DEFER_ACCEPT, &yes, sizeof(int));

          // Pin userspace thread on the same core for better cache affinity • cgroup, taskset, pthread_setaffinity_np(), ... any way you like
          //https://events.linuxfoundation.org/sites/events/files/slides/LinuxConJapan2016_makita_160712.pdf

          //remove iptable
          // modprobe -r iptable_filter
          //modprobe -r ip_tables
          // # sysctl -w net.ipv4.conf.all.rp_filter=0
          // # sysctl -w net.ipv4.conf.<NIC>.rp_filter=0
          //# sysctl -w net.ipv4.conf.all.accept_local=1
          //systemctl disable auditd; reboot

          // int pmtu = IP_PMTUDISC_DO;
          // setsockopt(sock, IPPROTO_IP, IP_MTU_DISCOVER, &pmtu, sizeof(pmtu));

          //manage +Hyper threading, cpu-thread-to-nic-queue

    s = bind( sfd, rp->ai_addr, rp->ai_addrlen );
    if ( s == 0 )
    {
      /* We managed to bind successfully! */
      break;
    }
    //maybe other cleaning as well ?
    close( sfd );
  }

  // Fail if nothing worked
  if ( rp == NULL )
    return_error ( "Could not bind\n" );

  // Clean up
  freeaddrinfo( result );

  // And return our server file descriptor
  return sfd;
}





static int
make_socket_non_blocking( int sfd )
{
  int flags, s;

  // Get the current flags on this socket
  flags = fcntl( sfd, F_GETFL, 0 );
  if ( flags == -1 )
    return_error ( "fcntl" );

  // Add the non-block flag
  flags |= O_NONBLOCK;
  s = fcntl( sfd, F_SETFL, flags );
  if ( s == -1 )
    return_error ( "fcntl" );

  return 0;
}




static int
accept_new_client( int sfd )
{
  struct sockaddr in_addr;
  socklen_t in_len;
  int infd, s;
  char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

  // Grab an incoming connection socket and its address
  in_len = sizeof in_addr;
  infd = accept( sfd, &in_addr, &in_len );
  if ( infd == -1 )
  {
    // If nothing was waiting
    if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ) )
    {
      // We have processed all incoming connections.
      return -1;
    }
    else
      return_error ( "accept" );
  }

  // Translate that sockets address to host/port
  s = getnameinfo( &in_addr, in_len,
    hbuf, sizeof hbuf,
    sbuf, sizeof sbuf,
    NI_NUMERICHOST | NI_NUMERICSERV );
  if ( s == 0 )
  {
    printf ( "Accepted connection on descriptor %d "
      " ( host=%s, port=%s )\n", infd, hbuf, sbuf );
  }

  /* Make the incoming socket non-blocking (required for
     epoll edge-triggered mode */
  s = make_socket_non_blocking( infd );
  if ( s == -1 )
    handle_error ( "Could not make socket non-blocking\n" );

  return infd;
}



int main(int argc, char *argv[])
{
  printf("simple server\n");

  int listenfd = 0;
  char *port = PORT;
  listenfd = create_and_bind( port );
  if ( listenfd == -1 )
    handle_error ( "Could not create and bind listener socket.\n" );

  // Make server socket non-blocking
  int s = make_socket_non_blocking( listenfd );
  if ( s == -1 )
    handle_error ( "Could not make listener socket non-blocking.\n" );

  // Mark server socket as a listener, with maximum backlog queue
  s = listen( listenfd, SOMAXCONN); //options.socket_backlog );
  if ( s == -1 )
    handle_error ( "Could not listen.\n" );

    int epollfd = 0;
    struct epoll_event ev, events[MAX_EVENTS];
    epollfd = epoll_create(MAX_EVENTS);
     if (epollfd == -1) {
        perror("epoll_create");
        exit(EXIT_FAILURE);
     }

     ev.events = EPOLLIN;
     ev.data.fd = listenfd;
     if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
     }


    char *message , client_message[2000];
    int read_size = 0;

    int client_sock = -1;
    while(1) {  // main accept() loop

      int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
       if (nfds == -1) {
        perror("epoll_pwait");
        exit(EXIT_FAILURE);
       }

       for (int n = 0; n < nfds; ++n) {
         printf("got a event: %d\n", events[n].data.fd );
         if (events[n].data.fd == listenfd) {

            client_sock = accept_new_client(listenfd);
              if (client_sock == -1) {
                  perror("accept");
                  //continue;
              } else {
                printf("we got new client\n");

                while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
                {
                  printf("recv data (size= %d), sending it back\n", read_size);
                  //Send the message back to client
                  // sleep(1);
                  write(client_sock , client_message , strlen(client_message));
                }
                if(read_size == 0)
                {
                    printf("Client disconnected");
                    close(client_sock);
                    // close(listenfd)
                }
                else if(read_size == -1)
                {
                    if (errno != EAGAIN)
                        {
                          perror ("read");
                          //we should wait for client to close, but we close it immediately
                          close(client_sock)
                        } else {
                          perror("recv failed");
                        }
                          close(client_sock);
                      break;

                }
                printf("end of recv");
                //Free the socket pointer
                // free((void*)client_sock);
              }
          } else {
              printf("%d\n", events[n].data.fd);
          }
        }
     }



  // create threads that will listen on accept and connection sockets with epoll
  return EXIT_SUCCESS;
}

/*
  we need 2 events types,
    one for  accept connection
    second for sending/receiving data
    maybe also for closing connection

    what about dead connection


    recv or read
        The accepted socket (the fd argument)
        The memory location where we want data to be collected (the second and the third arguments)
        The number of bytes read (the response)


    The non-blocking tcp server can be implemented as follows:

      Create a socket
      Mark it as non-blocking (this will make even the accept call non-blocking)
      Bind it to an address
      Listen on the socket
      Create an epoll instance
      Add your socket to the epoll instance (this way the incoming requests can be monitored through event notification)
      Create a read event queue
      Create threads for processing tasks from read queue
      Create a write event queue
      Create threads for processing tasks in the write queue
      Wait for events on epoll instance in a loop
      For incoming requests events
      call accept
      mark the accepted socket as non-blocking
      add it to the epoll instance for monitoring
      For read events, push the file descriptor and user data to read event queue
      For write events, push the file descriptor and user data to write event queue
      For close events, remove the file descriptor from the epoll instance


      4cpu -> accept, read, process, write



      timeout
        https://stackoverflow.com/questions/6590531/how-do-i-implement-epoll-timeout


        timeout = expirylist->expire_time - current_time();
        n_events = epoll_wait(epfd, events, maxevents, timeout);

        handle_events(events, n_events);

        for (client = expirylist; client != NULL && client->expire_time < current_time(); client = client->expire_next)
        {
            do_timeout(client);
        }

        or
      https://stackoverflow.com/questions/10772208/epoll-and-timeouts
        ou can create a recurring timerfd and add it to your epoll set.
        It will wake you up however often you like, at which point you can check all your client connections and drop the ones you think are stale.


        struct sockstamp_s {
          int sockfd;

          struct timeval tv; // last active
      };

      // check which socket has been inactive
      for (struct sockstamp_s *i = socklist; ...; i = next(i)) {
          if (diff(s->tv, now()) > 500) {
              // socket s->sockfd was inactive for more than 500 ms
              ...
          }
      }

      OR

      Try pairing each socket with a timer fd object (timerfd_create).
      For each socket in your application, create a timer that's initially set to expire after 500ms, and add the timer to the epoll object (same as with a socket—via epoll_ctl and EPOLL_CTL_ADD). Then, whenever data arrives on a socket, reset that socket's associated timer back to a 500ms timeout.

      If a timer expires (because a socket has been inactive for 500ms) then the timer will become "read ready" in the epoll object and cause any thread waiting on epoll_wait to wake up. That thread may then handle the timeout for the timer's associated socket.

      http://developerweb.net/viewtopic.php?id=5702
      maxevents
      It was used to determine the initial hashtable size in the kernel's epoll
implementation, but newer versions use a RB-tree, so it's ignored.
      timeout
        There are (at least) two ways to do this, use per socket timers or mark and sweep.

      See also the epoll(7) manpage.
*/

// /https://techtake.info/2016/09/19/non-blocking-tcp-server-explained/
//https://github.com/millken/c-example/blob/master/epoll-example.c
//https://xdecroc.wordpress.com/2016/03/16/using-epoll-io-event-notification-to-implement-an-asynchronous-server-unfinished/
//https://stackoverflow.com/questions/6401642/dealing-with-listening-socket-by-epoll
//https://stackoverflow.com/questions/3664537/nonblocking-socket-recv-problem-while-using-it-with-epoll
