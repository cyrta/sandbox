
# The Sandbox - repository of experiments
-----------------------------------------

Experiments, tests, random babble. 

Mostly a collection of "Hello World", methods to try,
recipies to check
examples for various languages.

---------------------
inspiration from  0xfe
https://github.com/0xfe/experiments

and my deep feeling to have a storage place 
for my work tryies.

------------------------------------

### Git little cheatsheet
```
$ git config --global user.name "Your Name Comes Here"
$ git config --global user.email you@yourdomain.example.com

$ git config --global color.diff auto
$ git config --global color.status auto
$ git config --global color.branch auto
```
Ignore list in .gitignore (in project root). Contents:
```
# Lines starting with '#' are considered comments.
# Ignore any file named foo.txt.
foo.txt
# Ignore (generated) html files,
*.html
# except foo.html which is maintained by hand.
!foo.html
# Ignore objects and archives.
*.[oa]
ruby_sess.*
.*.swp
.hi
*.pyc

```
**Workflow:**

```
$ git branch newthingaby

$ git branch
  newthingaby
  * master

$ git checkout newthingaby
  ... do stuff ...

$ git commit -a

$ git checkout master

$ git merge newthingaby

(If conflicts, fix them, then submit.)

$ git branch -d newthingaby

$ git push

```


