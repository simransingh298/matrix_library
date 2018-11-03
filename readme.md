# Multithreaded Matrix multiplication library

numpy_style_main.cpp is the single threaded library.
to compile it, use the following command:
`g++ numpy_style_main.cpp -std=c++11 -O4 -march=native`

## Relevant Programs:
- numpy_style_main.cpp very generalised single threaded array lib
- nm.cpp integer single threaded array lib
- numpy_multi_thr.cpp multithreaded lib


## The list of functions that NEED to be implemented are:
### Single threaded:
- Elementwise
	- [X] Add
	- [ ] Subtract
	- [ ] Multiply
	- [ ] Divide
	- [ ] Fn
	- [ ] conjugate (can be done using fn, or directly)
- [ ] reshape
- [X] matmul
- [ ] FFT
- [ ] transpose
- [ ] convolution
- [ ] (optional (what an irony)) padding with some number n
- [ ] trace
- [ ] sum
- [ ] mean

### Multi threaded:
- Elementwise
	- [ ] Add
	- [ ] Subtract
	- [ ] Multiply
	- [ ] Divide
	- [ ] Fn
	- [ ] conjugate (can be done using fn, or directly)
- [ ] reshape
- [ ] matmul
- [ ] FFT
- [ ] transpose
- [ ] convolution
- [ ] (optional (what an irony)) padding with some number n
- [ ] trace
- [ ] sum
- [ ] mean