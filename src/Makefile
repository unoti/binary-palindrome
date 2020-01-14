FLAGS = -O3
#FLAGS = 

palindrome : palindrome.o
	cc $(FLAGS) -o palindrome palindrome.o

palindrome.o : palindrome.c
	cc $(FLAGS) -c palindrome.c

clean :
	rm palindrome palindrome.o
