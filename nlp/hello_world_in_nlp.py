
# inspired by: https://www.quora.com/Whats-the-Hello-World-of-NLP/answer/Roman-Trusov

from collections import defaultdict
 
vocab = defaultdict(int)
 
with open(corpus_path) as f:
	for line in f:
		words = line.strip().split() # don't need that \n in the end of the last word
		for w in words:
			vocab[w] += 1
	f.close() 

