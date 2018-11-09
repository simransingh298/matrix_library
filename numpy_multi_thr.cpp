// #include <iostream>
// #include <list>
// #include <memory>
// #include <chrono>
// #include <cassert>
#include <thread>
#include <bits/stdc++.h>
using namespace std;
#define PI 3.14159265358979323846
template <class T>
string list_to_string(list<T> l){
    string res = "[";
    for (auto val : l){
        res += to_string(val) + ",";
    }
    res += "]";
    return res;
}
template <class T>
string vec_to_string(vector<T> v){
	string res = "[";
	if(v.size()>0)res+=to_string(v[0]);
	for (int i=1; i<v.size(); i++){
		res += "," + to_string(v[i]) ;
	}
	res += "]";
	return res;
}
template <class T>
T prod_elems_in_list(list<T> &l){
    T res = T(1);
    for(auto elem : l){
        res *= elem;
    }
    return res;
}
template <class T>
T prod_elems_in_vector(vector<T> &v){
	T res = T(1);
	for (auto elem:v){
		res *= elem;
	}
	return res;
}
template <class T>
T validate_vec(vector<T> &v){
	for(auto elem:v){
		if (elem <= 0){
			return false;
		}
	}
	return true;
}
template <class T>
list<T> make_list_from_arr(T *arr){
    list<T> res(arr, arr+sizeof(arr)/sizeof(T));
    return res;
}
vector<unsigned long long int> prods(vector<unsigned int> shape){
	vector<unsigned long long int> prod_vec;
	unsigned long long int prod = 1;
	unsigned int dimns = shape.size();
	for (int i = 0; i < dimns; ++i)
	{
		prod *= shape[dimns-i-1];
		prod_vec.push_back(prod);
	}
	return prod_vec;
}
template <class T>
void adh(T* lhs, T* rhs0, T* rhs1, unsigned int num_elems){
	for(; num_elems > 0; num_elems--){
		*lhs = *rhs0 + *rhs1;
		lhs++;rhs0++;rhs1++;
	}
	return;
}

vector<unsigned long long int> ret_cum_shape(vector<unsigned int> & shape){
	vector<unsigned long long int> res(shape.size(), 1);
	for (int i = shape.size()-2; i>= 0; i--){
		res[i] = res[i+1]*(shape[i+1]);
	}
	return res;
}

vector<unsigned long long int> ret_cum_shape2(vector<unsigned int> & shape){
	vector<unsigned long long int> res(shape.size(), 1);
	for (int i = shape.size()-2; i>= 0; i--){
		res[i] = res[i+1]*(shape[i]);
	}
	return res;
}

string to_string(complex<double> &a){
	return "(" + to_string( real(a)) +    " + " + to_string(imag(a))  +    " i )" ;
	// return "(" + to_string( real(a) ==  (double ) 0  ? 0 : real(a) ) +    " + " + to_string(imag(a))  +    " i )" ;
}

template<class T>
void separate (vector<T> &a, int offset, int n) {
    vector<T> b(n/2);  			// get temp heap storage
    for(int i=0; i<n/2; i++)    // copy all odd elements to heap storage
        b[i] = a[offset+ i*2+1];
    for(int i=0; i<n/2; i++)    // copy all even elements to lower-half of a[]
        a[offset+ i] = a[offset+ i*2];
    for(int i=0; i<n/2; i++)    // copy all odd (from heap) to upper-half of a[]
        a[offset + i+n/2] = b[i];
    // delete[] b;                 // delete heap storage
}
template<class T>
void fft2 (vector<T> &X, int offset,int N) {
    if(N < 2) {
    } else {
        separate(X,offset, N);      // all evens to lower half, all odds to upper half
        fft2(X, offset,     N/2);   // recurse even items
        fft2(X, offset + N/2, N/2);   // recurse odd  items
        for(int k=0; k<N/2; k++) {
            T e = X[offset + k    ];   // even
            T o = X[offset + k+N/2];   // odd
            T w = exp( complex<double>(0,-2.*M_PI*k/N) );
            X[offset + k    ] = e + w * o;
            X[offset + k+N/2] = e - w * o;
        }
    }
}

template<class T>
class gen_arr{
	vector<unsigned int> shape;
	vector<unsigned long long int> cumulative_shape;
	int ndim;
	shared_ptr<T> arr;
	int arr_len;
	int offset;
	static void add_helper(T* lhs, T* rhs0, T* rhs1, unsigned int num_elems){
		for(int i = 0; i < num_elems; i++){
			lhs[i] = rhs0[i] + rhs1[i];
		}
		return;
	}
	static void sub_helper(T* lhs, T* rhs0, T* rhs1, unsigned int num_elems){
		for(int i = 0; i < num_elems; i++){
			lhs[i] = rhs0[i] - rhs1[i];
		}
		return;
	}
	static void mul_helper(T* lhs, T* rhs0, T* rhs1, unsigned int num_elems){
		for(int i = 0; i < num_elems; i++){
			lhs[i] = rhs0[i] * rhs1[i];
		}
		return;
	}
	static void div_helper(T* lhs, T* rhs0, T* rhs1, unsigned int num_elems){
		for(int i = 0; i < num_elems; i++){
			lhs[i] = rhs0[i] / rhs1[i];
		}
		return;
	}
	static void fn_helper(T* lhs, T* rhs0, T* rhs1, unsigned int num_elems, function<T(T,T)> &fn){
		for(int i = 0; i < num_elems; i++){
			lhs[i] = fn(rhs0[i], rhs1[i]);
		}
		return;
	}
	static T basic_op_add(T a, T b){
		return a + b;
	}
	static T basic_op_sub(T a, T b){
		return a - b;
	}
	static T basic_op_mul(T a, T b){
		return a * b;
	}
	static T basic_op_div(T a, T b){
		return a / b;
	}

public:
	gen_arr(){
		this->arr.reset();
		this->ndim = 0;
		this->arr_len = 0;
		this->offset = 0;
	}
	void init(vector<unsigned int> shape, bool alloc=true){
		assert(validate_vec(shape));assert(validate_vec(shape));
		this->shape = shape;
		this->cumulative_shape = ret_cum_shape(this->shape);
		this->ndim = shape.size();
		this->arr_len = prod_elems_in_vector(shape);
		if (alloc){
			this->arr = shared_ptr<T>(new T[arr_len], std::default_delete<T[]>());
		}
		else{
			this->arr.reset();
		}
		this->offset = 0;
	}
	gen_arr(vector<unsigned int> shape, T init_val, bool do_init=true, bool alloc=true){
		if (! alloc){
			printf("non alloc constructor called\n");
		}
		init(shape, alloc);
		if (do_init && alloc){
			fill_n(arr.get(), this->arr_len, init_val);
		}
	}
	// gen_arr(vector<unsigned int> shape, T init_val){
	// 	// assert(validate_vec(shape));
	// 	init(shape, true);
	// 	fill_n(arr.get(), this->arr_len, init_val);
	// 	cout << endl;
	// }
	gen_arr(vector<unsigned int> shape){
		init(shape);
	}
	string print_stats(){
		string res = "";
		res = res + "shape:\t" + vec_to_string(this->shape)+";\t";
		res = res + "ndim:\t" + to_string(this->ndim)+";\t";
		res = res + "arr_len:\t" + to_string(this->arr_len)+";\t";
		res = res + "offset:\t" + to_string(this->offset)+";\t";
		return res;
	}
	gen_arr & multi_threaded_op(gen_arr<T>  & rhs, int num_thr, function<T(T,T)> fn){
		assert(this->shape == rhs.shape);
		gen_arr<T> *res = new gen_arr<T>;
		res->init(this->shape, true);
		// gen_arr<T> res(this->shape);
		int num_elem = this->arr_len;
		int load_per_thr = num_elem / num_thr;
		int num_left = num_elem - load_per_thr * num_thr;
		int num_thrs = num_left > 0 ? num_thr + 1 : num_thr;
		printf("num_elem:%d;load_per_thr:%d;num_left:%d;num_thr:%d,num_thrs:%d\n", num_elem, load_per_thr, num_left, num_thr, num_thrs);
		thread workers[num_thrs];
		// gen_arr<T> res(this->shape);
		T *lhsptr, *rhs0ptr, *rhs1ptr;
		lhsptr = res->arr.get();
		rhs0ptr = this->arr.get()+this->offset;
		rhs1ptr = rhs.arr.get()+rhs.offset;
		for (int i = 0; i < num_thr; i++){
			workers[i] = thread(&gen_arr<T>::fn_helper, lhsptr, rhs0ptr, rhs1ptr, load_per_thr, fn);
			// workers[i] = thread(&gen_arr<T>::add_helper, lhsptr, rhs0ptr, rhs1ptr, load_per_thr);
			lhsptr+= load_per_thr;
			rhs0ptr += load_per_thr;
			rhs1ptr += load_per_thr;
		}
		if (num_left > 0){
			workers[num_thr] = thread(&gen_arr<T>::fn_helper, lhsptr, rhs0ptr, rhs1ptr, num_left, fn);
			// workers[num_thr] = thread(&gen_arr<T>::add_helper, lhsptr, rhs0ptr, rhs1ptr, num_left);
		}
		for (int i = 0; i < num_thrs; i++){
			workers[i].join();
		}
		return *res;
	}

	gen_arr & multi_threaded_op2(gen_arr<T>  & rhs, int num_thr, string name){
		assert(this->shape == rhs.shape);
		gen_arr<T> *res = new gen_arr(this->shape);
		// gen_arr<T> res(this->shape);
		int num_elem = this->arr_len;
		int load_per_thr = num_elem / num_thr;
		int num_left = num_elem - load_per_thr * num_thr;
		int num_thrs = num_left > 0 ? num_thr + 1 : num_thr;
		printf("num_elem:%d;load_per_thr:%d;num_left:%d;num_thr:%d,num_thrs:%d\n", num_elem, load_per_thr, num_left, num_thr, num_thrs);
		thread workers[num_thrs];
		// gen_arr<T> res(this->shape);
		T *lhsptr, *rhs0ptr, *rhs1ptr;
		lhsptr = res->arr.get();
		rhs0ptr = this->arr.get()+this->offset;
		rhs1ptr = rhs.arr.get()+rhs.offset;
		function<void(T*,T*, T*, unsigned int)> fun_ptr;
		// void (fun_ptr(T*,T*, T*, unsigned int));
		// T* lhs, T* rhs0, T* rhs1, unsigned int num_elems
		if (name=="add"){
			fun_ptr= gen_arr<T>::add_helper;
		}
		else if (name=="sub"){
			fun_ptr = gen_arr<T>::sub_helper;
		}
		else if (name=="mul"){
			fun_ptr = gen_arr<T>::mul_helper;
		}
		else {
			fun_ptr = gen_arr<T>::div_helper;
		}
		for (int i = 0; i < num_thr; i++){
			workers[i] = thread(fun_ptr, lhsptr, rhs0ptr, rhs1ptr, load_per_thr);
			// workers[i] = thread(&gen_arr<T>::add_helper, lhsptr, rhs0ptr, rhs1ptr, load_per_thr);
			lhsptr+= load_per_thr;
			rhs0ptr += load_per_thr;
			rhs1ptr += load_per_thr;
		}
		if (num_left > 0){
			workers[num_thr] = thread(&adh<T>, lhsptr, rhs0ptr, rhs1ptr, num_left);
			// workers[num_thr] = thread(&gen_arr<T>::add_helper, lhsptr, rhs0ptr, rhs1ptr, num_left);
		}
		for (int i = 0; i < num_thrs; i++){
			workers[i].join();
		}
		return *res;
	}
	gen_arr & multi_threaded_add(gen_arr<T>  & rhs, int num_thr){
		return multi_threaded_op2(rhs, num_thr, "add");
	}
	gen_arr & multi_threaded_sub(gen_arr<T> &rhs, int num_thr){
		return multi_threaded_op2(rhs, num_thr, "sub");
	}
	gen_arr & multi_threaded_mul(gen_arr<T> &rhs, int num_thr){
		return multi_threaded_op2(rhs, num_thr, "mul");
	}
	gen_arr & multi_threaded_div(gen_arr<T> &rhs, int num_thr){
		return multi_threaded_op2(rhs, num_thr, "div");
	}

	static void min_max_thr(T* rhs, T *res, int num_elem, string fn){
		*res = *rhs;
		for(int i = 0; i < num_elem; i++)
		{	
			if(fn == "min"){
				if(*rhs < *res)
					*res = *rhs;
			}

			else if(fn == "max"){
				if (*rhs > *res)
					*res = *rhs;
			}
			rhs++;
		}

		
	}

	T  min_max_value(int num_thr, string fn){
		int num_elem = this->arr_len/num_thr;

		int num_left = arr_len - num_elem * num_thr;
		int num_thrs = num_left > 0 ? num_thr + 1 : num_thr;

		vector<unsigned int> res_shape;
		
		res_shape.push_back(1);
		res_shape.push_back(num_thrs);
		gen_arr *res_arr = new gen_arr(res_shape);

		thread workers[num_thrs];
		T *rhsptr, *res;
		res = res_arr->arr.get();

		rhsptr = this->arr.get() + this->offset;

		T ans = *rhsptr;

		for(int i = 0; i < num_thr; i++)
		{
			workers[i] = thread(&min_max_thr, rhsptr, res, num_elem,fn);
			rhsptr += num_elem;
			res++;
			
		}

		if(num_left > 0){
			workers[num_thr] = thread(&min_max_thr, rhsptr, res, num_left, fn);
			res++;
		}

		res -= num_thrs;

		for (int i = 0; i < num_thrs; i++)
		{
			workers[i].join();
			if(fn == "min"){
				if(*res<ans)
					ans = *res;
			}

			else if (fn == "max"){
				if (*res > ans)
					ans = *res;
			}			

			res++;
		}
		

		return ans;
	}

	T min_value(int num_thr){
		return min_max_value(num_thr, "min");
	}

	T max_value(int num_thr){
		return min_max_value(num_thr, "max");
	}

	static void trace_single_thr(T* inp, T* res, int load, int m, int jump){
		
		for(int i = 0; i < load; i++)
		{
			T temp = (T) 0;
			for(int j = 0; j < m; j++)
			{
				temp += *inp;
				inp += jump;
			}

			inp -= jump*m; inp++;
			*res = temp;
			res++;
			
		}

		
	}

	static void add_single_col(T *inp, T *arr, int jump, int load_col){

		
		T temp = (T) 0;
		for (int j = 0; j < load_col; j++)
		{
			temp += *inp;
			inp += jump;
		}

		*arr = temp;
	
	}

	gen_arr trace(int num_thr){
		vector<unsigned int> res_shape;
		vector<unsigned int> temp = this->shape;

		int m1 = temp.front(); temp.erase(temp.begin());
		int m2 = temp.front(); temp.erase(temp.begin());

		int m = min(m1, m2);

		res_shape.push_back(1);
		while(!temp.empty()){
			res_shape.push_back(temp.front());
			temp.erase(temp.begin());
		}

		gen_arr<T> *res = new gen_arr(res_shape);

		int num_elem = this->arr_len;

		T *lhsptr, *rhsptr;
		lhsptr = res->arr.get();
		rhsptr = this->arr.get() + this->offset;

		int res_shape_prod = prod_elems_in_vector(res->shape) ;
		int load = res_shape_prod/ num_thr;
		int load_left = res_shape_prod - (load * num_thr);

		if(load > 0){
			int num_thrs = load_left > 0 ? num_thr + 1 : num_thr;
			thread workers[num_thrs];

			long long int jump = res_shape_prod * (m2 + 1);
			for (int i = 0; i < num_thr; i++)
			{
				workers[i] = thread(&trace_single_thr, rhsptr, lhsptr, load, m, jump);
				rhsptr += load;
				lhsptr += load;
			}

			if(load_left > 0){
				workers[num_thr] = thread(&trace_single_thr, rhsptr, lhsptr, load_left, m, jump);
			}

			for (int i = 0; i < num_thrs; i++)
			{	
				workers[i].join();
			}
		}

		else{
			load = num_thr / res_shape_prod;
			load_left = num_thr - load * res_shape_prod;
			int col = load_left == 0 ? res_shape_prod : res_shape_prod -1;

			thread workers[num_thr];

			int jump = res_shape_prod * (m2 + 1);

			T arr[num_thr];

			int thr = 0;

			for(int  j = 0; j < col; j++){
				int load_col = m / load;
				int load_col_left = m - load_col * (load - 1);

				for (int i = 0; i < load - 1; i++)
				{
					workers[thr] = thread(&add_single_col, rhsptr, &(arr[thr]), jump, load_col);
					rhsptr += jump * load_col;
					thr++;
				}

				workers[thr] = thread(&add_single_col, rhsptr, &(arr[thr]), jump, load_col_left);
				thr++;

				rhsptr -= jump * load_col * (load - 1);
				rhsptr++;
			}

			if(load_left > 0){
				load += load_left;
				int load_col = ceil(m / load);
				int load_col_left = m - load_col * (load - 1);

				for (int i = 0; i < load - 1; i++)
				{
					workers[thr] = thread(&add_single_col, rhsptr, &(arr[thr]), jump, load_col);
					rhsptr += jump * load_col;
					thr++;
					
				}

				workers[thr] = thread(&add_single_col, rhsptr, &(arr[thr]), jump, load_col_left);
				thr++;
				

				rhsptr -= jump * load_col * (load - 1);
				rhsptr++;
						
			}


			load = num_thr / res_shape_prod;
			for (int k = 0; k < num_thr; k++)
			{
				workers[k].join();
				if (k % load == 0 and k != 0 and k <= col*load){
					int a = k - load;
					*lhsptr = (T)0;
					for(int j = 0; j < load; j++){
						*lhsptr += arr[a+j];
					}
					lhsptr++;
					
				}

				if(k == num_thr-1){
					load += load_left;
					int a = k - load + 1;
					*lhsptr = (T)0;
					for (int j = 0; j < load; j++)
					{
						*lhsptr += arr[a + j];
					}
					lhsptr++;
				}


			}
		}
		return *res;
	}

	static void sum_axis0_thr(T *rhs, T *res, int num_elem)
	{
		*res = *rhs;
		rhs++;
		for (int i = 1; i < num_elem; i++)
		{
			*res += *rhs;
			rhs++;
		}
	}

	static void add_single_thr(T *inp, T *res, int load, int m, int jump, int pos, int pos_jmp)
	{
		for (int i = 0; i < load; i++)
		{
			T temp = *inp;
			inp += jump;
			for (int j = 1; j < m; j++)
			{
				temp += *inp;
				inp += jump;
			}
			
			pos++;
			inp -= jump * m;
			if (pos >= jump){
				inp += (pos_jmp);
				pos = pos % jump;
			}
			else{
				inp++;
			}
			*res = temp;
			res++;
		}
	}

	gen_arr sum(int num_thr, int axis = -1,  string mean = "no mean"){

		int dim = this->ndim;

		assert(axis < dim);
		assert(axis >= -1 );


		if(axis == -1){

			int num_elem = this->arr_len / num_thr;
			int num_left = arr_len - num_elem * num_thr;
			int num_thrs = num_left > 0 ? num_thr + 1 : num_thr;

			vector<unsigned int> res_shape;

			thread workers[num_thrs];
			T *rhsptr, *res, *ans;
			rhsptr = this->arr.get() + this->offset;

			res_shape.push_back(1);
			res_shape.push_back(num_thrs);
			gen_arr *res_arr = new gen_arr(res_shape);

			vector<unsigned int> ans_shape;
			ans_shape.push_back(1);
			gen_arr *ans_arr = new gen_arr(ans_shape);

			res = res_arr->arr.get();
			ans = ans_arr->arr.get();
			for (int i = 0; i < num_thr; i++)
			{
				workers[i] = thread(&sum_axis0_thr, rhsptr, res, num_elem);
				rhsptr += num_elem;
				res++;
			}

			if (num_left > 0)
			{
				workers[num_thr] = thread(&sum_axis0_thr, rhsptr, res, num_left);
				res++;
			}

			res -= num_thrs;

			for (int i = 0; i < num_thrs; i++)
			{
				workers[i].join();
				if(i == 0)
					*ans = *res;
				else
					*ans += *res;

				res++;
			}

			if(mean == "with mean"){
				*ans /= this->arr_len;
			}

			return *ans_arr;
		}
		else{
			vector<unsigned int> res_shape;
			vector<unsigned int> temp = this->shape;

			int m ;
			int sz = temp.size();		
			for(int i = 0; i < sz; i++){
				if(i==axis){
					m=temp.front();
				}
				else{
					res_shape.push_back(temp.front());
				}
				temp.erase(temp.begin());	
			}

			gen_arr<T> *res = new gen_arr(res_shape);

			int num_elem = this->arr_len;

			T *lhsptr, *rhsptr;
			lhsptr = res->arr.get();
			rhsptr = this->arr.get() + this->offset;

			int res_shape_prod = prod_elems_in_vector(res->shape);
			int load = res_shape_prod / num_thr;
			int load_left = res_shape_prod - (load * num_thr);

			temp = this->shape;
			
			for(int i = 0; i <= axis; i++){
				temp.erase(temp.begin());
			}
			long long int jump = prod_elems_in_vector(temp);

			if (load > 0)
			{	
				int num_thrs = load_left > 0 ? num_thr + 1 : num_thr;
				thread workers[num_thrs];
				int pos = 0;
				int pos_jmp = (m-1)*jump +1 ;
				for (int i = 0; i < num_thr; i++)
				{
					workers[i] = thread(&add_single_thr, rhsptr, lhsptr, load, m, jump, pos, pos_jmp);
					pos+=load;
					lhsptr += load;
					if(pos >= jump){
						int ps_jp = pos/jump;
						int jp = load - ps_jp;
						pos = pos %jump;

						rhsptr += (pos_jmp*ps_jp);
						rhsptr += jp;
					}
					else{
						rhsptr += load;
					}
				}

				if (load_left > 0)
				{
					workers[num_thr] = thread(&add_single_thr, rhsptr, lhsptr, load_left, m, jump, pos, pos_jmp);
				}

				for (int i = 0; i < num_thrs; i++)
				{
					workers[i].join();
				}
			}

			else
			{
				cout<<"enter"<<endl;
				load = num_thr / res_shape_prod;
				load_left = num_thr - load * res_shape_prod;
				int col = load_left == 0 ? res_shape_prod : res_shape_prod - 1;

				thread workers[num_thr];

				T arr[num_thr];

				int thr = 0;

				int pos = 0;
				int pos_jmp = (m-1)*jump + 1;

				for (int j = 0; j < col; j++)
				{
					int load_col = m / load;
					int load_col_left = m - load_col * (load - 1);

					for (int i = 0; i < load - 1; i++)
					{
						workers[thr] = thread(&add_single_col, rhsptr, &(arr[thr]), jump, load_col);
						rhsptr += jump * load_col;
						thr++;
					}

					workers[thr] = thread(&add_single_col, rhsptr, &(arr[thr]), jump, load_col_left);
					thr++;

					rhsptr -= jump * load_col * (load - 1);
					pos++;
					if(pos >= jump){
						rhsptr+= pos_jmp;
						pos = pos%jump;
					}
					else{
						rhsptr++;
					}
				}
				if (load_left > 0)
				{
					load += load_left;
					int load_col = m / load;
					int load_col_left = m - load_col * (load - 1);
					cout<<load_col_left<<endl<<load_col<<endl<<load<<endl;

					for (int i = 0; i < load - 1; i++)
					{
						workers[thr] = thread(&add_single_col, rhsptr, &(arr[thr]), jump, load_col);
						rhsptr += jump * load_col;
						thr++;
					}

					workers[thr] = thread(&add_single_col, rhsptr, &(arr[thr]), jump, load_col_left);
				}

				load = num_thr / res_shape_prod;
				for (int k = 0; k < num_thr; k++)
				{
					workers[k].join();
					if (k % load == 0 and k != 0 and k <= col * load)
					{
						int a = k - load;
						*lhsptr = (T) 0;
						for (int j = 0; j < load; j++)
						{
							*lhsptr += arr[a + j];
						}
						if(mean == "with mean"){
							*lhsptr /= m;
						}
						lhsptr++;
					}

					if (k == num_thr - 1)
					{
						load += load_left;
						int a = k - load + 1;
						*lhsptr = (T)0;
						for (int j = 0; j < load; j++)
						{
							*lhsptr += arr[a + j];
						}
						if (mean == "with mean")
						{
							*lhsptr /= m;
						}
						lhsptr++;
					}
				}
			}
			return *res;
		}
	}

	gen_arr mean(int num_thr, int axis = -1)
	{
		assert(axis >= -1);
		assert(axis < this->ndim);

		gen_arr res = this->sum(num_thr, axis, "with mean");

		return res;
	}
	/* gen_arr multi_threaded_add(gen_arr<T>  & rhs, int num_thr){
		assert(this->shape == rhs.shape);
		gen_arr<T> res(this->shape);
		int num_elem = this->arr_len;
		int load_per_thr = num_elem / num_thr;
		int num_left = num_elem - load_per_thr * num_thr;
		int num_thrs = num_left > 0 ? num_thr + 1 : num_thr;
		printf("num_elem:%d;load_per_thr:%d;num_left:%d;num_thr:%d,num_thrs:%d\n", num_elem, load_per_thr, num_left, num_thr, num_thrs);
		thread workers[num_thrs];
		// gen_arr<T> res(this->shape);
		T *lhsptr, *rhs0ptr, *rhs1ptr;
		lhsptr = res.arr.get();
		rhs0ptr = this->arr.get()+this->offset;
		rhs1ptr = rhs.arr.get()+rhs.offset;
		for (int i = 0; i < num_thr; i++){
			workers[i] = thread(&adh<T>, lhsptr, rhs0ptr, rhs1ptr, load_per_thr);
			// workers[i] = thread(&gen_arr<T>::add_helper, lhsptr, rhs0ptr, rhs1ptr, load_per_thr);
			lhsptr+= load_per_thr;
			rhs0ptr += load_per_thr;
			rhs1ptr += load_per_thr;
		}
		if (num_left > 0){
			workers[num_thr] = thread(&adh<T>, lhsptr, rhs0ptr, rhs1ptr, num_left);
			// workers[num_thr] = thread(&gen_arr<T>::add_helper, lhsptr, rhs0ptr, rhs1ptr, num_left);
		}
		for (int i = 0; i < num_thrs; i++){
			workers[i].join();
		}
		return res;
	} */
	gen_arr add(gen_arr<T> const &rhs){
		assert(this->shape == rhs.shape);
		gen_arr<T> res(this->shape);
		int ilhs = 0, irhs0 = this->offset, irhs1 = rhs.offset;
		T *ptrlhs = res.arr.get(), *ptrrhs0 = this->arr.get(), *ptrrhs1 = rhs.arr.get();
		for (; ilhs < this->arr_len; ilhs++, irhs0++, irhs1++){
			ptrlhs[ilhs] = ptrrhs0[irhs0] + ptrrhs1[irhs1];
			// res.arr.get()[ilhs] = this->arr.get()[irhs0] + rhs.arr.get()[irhs1];
		}
		return res;
	}
	gen_arr sub(gen_arr<T> const &rhs){
		assert(this->shape == rhs.shape);
		gen_arr<T> res(this->shape);
		int ilhs = 0, irhs0 = this->offset, irhs1 = rhs.offset;
		T *ptrlhs = res.arr.get(), *ptrrhs0 = this->arr.get(), *ptrrhs1 = rhs.arr.get();
		for (; ilhs < this->arr_len; ilhs++, irhs0++, irhs1++){
			ptrlhs[ilhs] = ptrrhs0[irhs0] - ptrrhs1[irhs1];
			// res.arr.get()[ilhs] = this->arr.get()[irhs0] + rhs.arr.get()[irhs1];
		}
		return res;
	}
	gen_arr mul(gen_arr<T> const &rhs){
		assert(this->shape == rhs.shape);
		gen_arr<T> res(this->shape);
		int ilhs = 0, irhs0 = this->offset, irhs1 = rhs.offset;
		T *ptrlhs = res.arr.get(), *ptrrhs0 = this->arr.get(), *ptrrhs1 = rhs.arr.get();
		for (; ilhs < this->arr_len; ilhs++, irhs0++, irhs1++){
			ptrlhs[ilhs] = ptrrhs0[irhs0] * ptrrhs1[irhs1];
			// res.arr.get()[ilhs] = this->arr.get()[irhs0] + rhs.arr.get()[irhs1];
		}
		return res;
	}
	gen_arr div(gen_arr<T> const &rhs){
		assert(this->shape == rhs.shape);
		gen_arr<T> res(this->shape);
		int ilhs = 0, irhs0 = this->offset, irhs1 = rhs.offset;
		T *ptrlhs = res.arr.get(), *ptrrhs0 = this->arr.get(), *ptrrhs1 = rhs.arr.get();
		for (; ilhs < this->arr_len; ilhs++, irhs0++, irhs1++){
			ptrlhs[ilhs] = ptrrhs0[irhs0] / ptrrhs1[irhs1];
			// res.arr.get()[ilhs] = this->arr.get()[irhs0] + rhs.arr.get()[irhs1];
		}
		return res;
	}
	gen_arr  operator+(gen_arr const &rhs){
        return this->add(rhs);
    }
	gen_arr  operator-(gen_arr const &rhs){
        return this->sub(rhs);
    }
	gen_arr  operator*(gen_arr const &rhs){
        return this->mul(rhs);
    }
	gen_arr  operator/(gen_arr const &rhs){
        return this->div(rhs);
    }
	static void transpose_helper(T*lhsptr, T*rhsptr, int ts0, int ts1, int outnum){
		for (int i = 0; i < outnum; i++){
			for (int j = 0; j < ts1; j++){
				*lhsptr = *rhsptr;
				lhsptr += ts0;
				rhsptr++;
			}
			lhsptr -= ts0 * ts1 - 1;
		}
	}
	gen_arr & transpose_mt(int num_thr){
		assert(this->ndim == 2);
		assert(num_thr > 0);
		gen_arr<T> *rest = new gen_arr<T>;
		gen_arr<T> &res = *rest; // actual copying doesnt happen
		gen_arr<T> &me = *this;
		vector<unsigned int> newshape = shape;
		newshape[0] = this->shape[1];
		newshape[1] = this->shape[0];
		res.init(newshape, true);
		res.cumulative_shape = ret_cum_shape(res.shape);
		T *lhsptr = res.arr.get() + res.offset, *rhs0ptr = this->arr.get() + this->offset;
		int num_col = this->shape[0];
		int load_per_thr = num_col / num_thr;
		int num_left = num_col - load_per_thr * num_thr;
		int num_thrs = num_left > 0 ? num_thr + 1 : num_thr;
		printf("num_col:%d;load_per_thr:%d;num_left:%d;num_thr:%d,num_thrs:%d\n", num_col, load_per_thr, num_left, num_thr, num_thrs);
		thread workers[num_thrs];
		int ts0 = this->shape[0];
		int ts1 = this->shape[1];
		for (int i = 0; i < num_thr; i++){
			workers[i] = thread(&gen_arr<T>::transpose_helper, lhsptr, rhs0ptr, ts0, ts1, load_per_thr);
			// workers[i] = thread(&gen_arr<T>::add_helper, lhsptr, rhs0ptr, rhs1ptr, load_per_thr);
			lhsptr += load_per_thr;
			rhs0ptr += load_per_thr*ts1;
		}
		if (num_left > 0){
			workers[num_thr] = thread(&gen_arr<T>::transpose_helper, lhsptr, rhs0ptr, ts0, ts1, num_left);
			lhsptr += num_left; // not needed, just for completeness
			rhs0ptr += num_left * ts1;
			// workers[num_thr] = thread(&gen_arr<T>::add_helper, lhsptr, rhs0ptr, rhs1ptr, num_left);
		}
		for (int i = 0; i < num_thrs; i++){
			workers[i].join();
		}
		return res;
	}
	gen_arr & transpose(){
		assert(this->ndim == 2);
		gen_arr<T> *rest = new gen_arr<T>;
		gen_arr<T> &res = *rest; // actual copying doesnt happen
		gen_arr<T> &me = *this;
		vector<unsigned int> newshape = shape;
		newshape[0] = this->shape[1];
		newshape[1] = this->shape[0];
		res.init(newshape, true);
		res.cumulative_shape = ret_cum_shape(res.shape);
		T *lhsptr = res.arr.get()+res.offset, *rhsptr = this->arr.get()+this->offset;
		register int ts0 = this->shape[0];
		register int ts1 = this->shape[1];
		for (int i = 0; i < ts0; i++){
			for (int j = 0; j < ts1; j++){
				*lhsptr = *rhsptr;
				lhsptr += ts0;
				rhsptr++;
				// res[j].getval(i) = *rhsptr;  // this also works but is slower
				// res[j][i].getval() = me[i][j].getval(); // this works but is much slower
			}
			lhsptr -= ts0*ts1 - 1;
		}
		return res;
	}
	void reshape(vector<unsigned int> newshape){
		assert(validate_vec(newshape));
		vector<unsigned long long int> newcumshape = ret_cum_shape(newshape);
		assert(this->arr_len == newcumshape[0]*newshape[0]);
		this->shape = newshape;
		this->cumulative_shape = newcumshape;
		this->ndim = this->shape.size();
		return;
	}
	gen_arr & operator=(gen_arr const & rhs){
		this->shape = rhs.shape;
		this->cumulative_shape = rhs.cumulative_shape;
		this->ndim = rhs.ndim;
		this->arr_len = rhs.arr_len;
		this->arr = shared_ptr<T>(new T[arr_len], std::default_delete<T>());
		memcpy(this->arr.get(), rhs.arr.get() + rhs.offset, this->arr_len * sizeof(T));
		this->offset = 0;
		return *this;
	}
	gen_arr& operator[](int index){ // see getval
		assert(index >= 0);
        if (this->ndim > 0){
			vector<unsigned int> new_shape(this->shape.begin()+1, this->shape.end());
			T temp;
			gen_arr<T> *res = new gen_arr<T>;
			res->init(new_shape, false);
			res->arr = this->arr;
			int new_offset = this->offset + this->cumulative_shape[0]*index;
			res->offset = new_offset;
			return *res;
		}
		else{
			printf("this->ndim should be a positive int\n");
			assert(this->ndim > 0);
		}
    }
	T & getval(int index){
		assert(index >= 0);
		assert(this->arr_len > index);
		assert(this->ndim == 1);
		return this->arr.get()[index+this->offset];
	}
	T & getval(){
		assert(this->ndim == 0);
		return this->arr.get()[this->offset];
	}
	gen_arr matmul(gen_arr<T> & rhs){
		assert(this->ndim == 2 && rhs.ndim == 2);
		int m1 = this->shape[0], n1 = this->shape[1];
		int m2 = rhs.shape[0], n2 = rhs.shape[1];
		assert(n1 == m2);
		vector<unsigned int> new_shape;
		new_shape.push_back(m1); new_shape.push_back(n2);
		gen_arr<T> res(new_shape, 0, false, true);
		printf("matmul: res defined\n");
		int sum;
		int rhs0off = this->offset, rhs1off = rhs.offset;
		int rhs0sh0 = this->cumulative_shape[0], rhs1sh0 = rhs.cumulative_shape[0];
		// cout << "rhs.cumulative_shape = " << vec_to_string(rhs.cumulative_shape) << endl;
		// cout << "rhs.shape = " << vec_to_string(rhs.shape) << endl;
		T *ptrlhs = res.arr.get(), *ptrrhs0 = this->arr.get(), *ptrrhs1 = rhs.arr.get();
		ptrlhs = ptrlhs + res.offset;
		for (int ii = 0; ii < m1; ii++){
			for(int jj = 0; jj < n2; jj++){
				register T sum = 0;
				for (int kk = 0; kk < n1; kk++){
					sum += ptrrhs0[rhs0off+ii*rhs0sh0+kk]*ptrrhs1[rhs1off+kk*rhs1sh0+jj];
				}
				*ptrlhs ++ = sum;
			}
		}
		return res;
	}

	string str(){
		cout << "str entered\n";
		auto prod_vec = prods(shape);
		// vector<unsigned long long int> prod_vec = ret_cum_shape(shape);
		// reverse(prod_vec.begin(), prod_vec.end());
		int dimns = shape.size();
		string res = "";
		for (int i = 0; i < prod_vec[dimns-1]; ++i){
			int count = 0;
			for (int j = 0; j < dimns; ++j){
				if(i % prod_vec[j] == 0){
					count++;
				}
				else{
					break;
				}
			}
			if(count){
				for (int j = 0; j < dimns; ++j){
					if(j >= dimns-count){
						res += "[";
					}
					else{
						res += " ";
					}
				}
			}
			res += to_string(this->arr.get()[i]);
			count = 0;
			for (int j = 0; j < dimns; ++j){
				if((i+1) % prod_vec[j] == 0){
					count++;
					res += "]";
				}
				else{
					break;
				}
			}
			if(count){
				res += "\n";
			}
			else{
				res+= " ";
			}
		}
		return res;
	}
	string dummy_dump(){
		string res = "[";
		int ulim = this->offset + this->arr_len;
		int* ptrarr = this->arr.get();
		for (int i = this->offset; i < ulim; i++){
			res += to_string(ptrarr[i]) + ",";
		}
		res[res.size()-1] = ']';
		return res;
	}
	vector<unsigned int> get_shape(){
		return this->shape;
	}
	gen_arr dft(){
		int ts=this->shape[0];
		gen_arr<complex<double> > w (vector<unsigned int > (1,ts));
		T *ptrw = w.arr.get() ;
		for(int i=0;i<ts;i++){
			*ptrw =0;
			T *ptr_mat = this->arr.get() ;
			for(int j=0;j<ts;j++){
				*ptrw += (* ptr_mat++ )*  polar(1.0,-2*PI*i*j/ts);
			}
			*ptrw++;
		}
		return  w;
	}
	vector<T> gen1_to_vector(){
		int size= this->get_shape()[0];
		vector<T> res(size);
		T * ptr = this->arr.get() + this->offset;
		for(int i=0;i<size;i++){
			res[i] = * ptr ++;
		}
		return res;
	}
	gen_arr<T> vector_gen1(vector<T> & v){
		int size = v.size();
		// vector<unsigned int> new_shape;
		// new_shape.push_back(size);
		gen_arr< T> res (vector<unsigned int> (1,size));
		T * ptr = res.arr.get();
		for(int i=0;i<size;i++){
			 * ptr ++ = v[i];
		}
		return res;
	}
	gen_arr<T> fft(){
		assert(this->shape.size() == 1  );
		int size = this->shape[0];
		assert(__builtin_popcount(size) == 1);
		vector<T> ans= gen1_to_vector();
		fft2(ans, 0, size);
		return vector_gen1(ans);
	}
	gen_arr dft_2(){
		int s1=this->shape[0];
		int s2=this->shape[1];
		double powe=-2.0*PI;
		gen_arr<complex <double> > w(this->shape);
		T *ptrw=w.arr.get();
		double mn = s1*s2*1.0;
		double i_by_s1,j_by_s2;
		for (int i = 0; i < s1; ++i)
		{
			for (int j = 0; j < s2; ++j)
			{	//w[i][j].getval()=0;
				*ptrw=0;
				i_by_s1=i*1.0/s1;
				j_by_s2=j*1.0/s2;
				T * ptr_mat = this->arr.get() + offset;
				for(int k=0;k<s1;k++){
					for(int l=0;l<s2;l++){
						*ptrw+= (* ptr_mat++ ) *polar(1.0,powe*(i_by_s1*k+j_by_s2*l));
					}
				}
				*ptrw/=sqrt(mn);
				// *ptrw/=mn;
				*ptrw++;
			}
		}
		return w;
	}


};






int main(int argc, char* argv[]){
	
	// vector<unsigned int > shape_dft;
	// shape_dft.push_back(4);
	// // shape_dft.push_back(1);
	// gen_arr<complex<double> > X =  (shape_dft);
	// X[0].getval()= 1;
    // X[1].getval()= complex<double >(2,-1);
    // X[2].getval()= complex<double >(0,-1);
    // X[3].getval()= complex<double >(-1,2);
	// // cout << "X's dump: \n" << X.str()<< endl;
	// auto t_stamp01 = chrono::high_resolution_clock::now();
	// gen_arr<complex< double > > ans = X.fft();
	// gen_arr<complex< double > > ans = X.dft();
	// auto t_stamp02 = chrono::high_resolution_clock::now();
	// auto time_021 = chrono::duration_cast<chrono::duration<double>>(t_stamp02 - t_stamp01).count();
	// cout<<ans.str()<<endl;
	// cout<<ans[0].getval()<<endl;
	// cout<<ans[1].getval()<<endl;
	// cout<<ans[2].getval()<<endl;
	// cout<<ans[3].getval()<<endl;
	// // cout<<ans[0].getval()<<endl;
	// cout<<"tim taken "<< time_021<<endl;
	// vector<complex<double> > temp = X.gen1_to_vector();

	// for(int i=0;i<temp.size();i++)
	// 	cout<<temp[i]<<endl;
	
	// fft2( temp,0,4);


	// for(int i=0;i<temp.size();i++)
	// 	cout<<temp[i]<<endl;?


	vector<unsigned int> shape1;
	shape1.push_back(600);
	shape1.push_back(600);
	shape1.push_back(500);
	shape1.push_back(5);

	gen_arr<int > v(shape1,(0.0,0.0));

	// int temp = 0;
	// for(int i = 0; i < 600; i++){
	// 	for(int j = 0; j < 600; j++){
	// 		for (int k = 0; k < 5; k++){
	// 			for (int l = 0; l < 5; l++){
	// 				v[i][j][k][l].getval() = temp;
	// 				temp++;
	// 			}
	// 		}
	// 	}
	// }

	v[1][1][1][1].getval() = -58;
	

	// v[1][2].getval()=70;
	// v[1][3].getval()=80;
	// v[1][4].getval()=90;
	// v[2][2].getval()=90;
	// v[2][3].getval()=100;
	// v[2][4].getval()=110;
	// v[3][2].getval()=110;
	// v[3][3].getval()=120;
	// v[3][4].getval()=130;
	// v[4][2].getval()=130;
	// v[4][3].getval()=140;
	// v[4][4].getval()=150;

	// gen_arr<complex<double> > ans = v.dft_2();
	int aa = v.min_value(1);
	int ab = v.max_value(1);

	gen_arr< int> ans = v.mean(200,2);

	// cout<<ans.shape<<endl;
	cout<<aa<<endl<<ab<<endl;
	cout<<vec_to_string(ans.get_shape())<<endl;
	// cout<<ans.str();







	// int num_thr = 4;
	// if (argc >= 2){
	// 	num_thr = atoi(argv[1]);
	// }
	// int shapearr[2] = {4,1};
	// vector<vector<unsigned int> > shapes;
	// shapes.push_back(vector<unsigned int> ());
	// shapes[0].push_back(1000);shapes[0].push_back(1000);
	// shapes.push_back(vector<unsigned int> ());
	// shapes[1].push_back(1000);shapes[1].push_back(1000);
	// shapes.push_back(vector<unsigned int> ());
	// shapes[2].push_back(3);shapes[2].push_back(4);
	// // shapes[0].push_back(1000);shapes[0].push_back(4000);
	// // vector<unsigned int> shape(shapearr, shapearr+sizeof(shapearr)/sizeof(shapearr[0]));
	// auto t_stamp01 = chrono::high_resolution_clock::now();
	// gen_arr<int> a0(shapes[0], 9), b0(shapes[0], 2);
	// auto t_stamp02 = chrono::high_resolution_clock::now();
	// gen_arr<int> c0 = a0 + b0;
	// auto t_stamp03 = chrono::high_resolution_clock::now();
	// auto alloc_time021 = chrono::duration_cast<chrono::duration<double>>(t_stamp02 - t_stamp01).count();
	// auto alloc_time032 = chrono::duration_cast<chrono::duration<double>>(t_stamp03 - t_stamp02).count();

	// cout << "c0.shape" << vec_to_string(c0.get_shape()) << endl;
	// printf("Time(in seconds): Alloc:%f;\tComputation(add):%f;\n", alloc_time021, alloc_time032);

	// auto t_stamp11 = chrono::high_resolution_clock::now();
	// gen_arr<int> a1(shapes[0], 3), b1(shapes[0], 7);
	// auto t_stamp12 = chrono::high_resolution_clock::now();
	// /* 
	// auto c1 = a1.matmul(b1);
	// auto c1 = a1.multi_threaded_add2(b1, num_thr);
	// auto c1 = a1.multi_threaded_op2(b1, num_thr, "add");
	// auto c1 = a1.multi_threaded_add(b1, num_thr); 
	// */
	// auto c1 = a1.multi_threaded_add(b1, num_thr);
	// // printf("c2 calculated\n");
    // auto t_stamp13 = chrono::high_resolution_clock::now();
	// auto alloc_time121 = chrono::duration_cast<chrono::duration<double>>(t_stamp12 - t_stamp11).count();
	// auto alloc_time132 = chrono::duration_cast<chrono::duration<double>>(t_stamp13 - t_stamp12).count();
	// // t_stamp22 = chrono::high_resolution_clock::now();
	// // c2 = a2.add(b2);
	// // t_stamp23 = chrono::high_resolution_clock::now();
	// // // auto alloc_time321 = chrono::duration_cast<chrono::duration<double>>(t_stamp22 - t_stamp21).count();
	// // auto alloc_time332 = chrono::duration_cast<chrono::duration<double>>(t_stamp23 - t_stamp22).count();
	// cout << "fooo\n";
	// cout << "c1.shape" << vec_to_string(c1.get_shape()) << endl;
	// printf("Time(in seconds): Alloc:%f;\tComputation(mult add):%f;\n", alloc_time121, alloc_time132);

	// auto t_stamp21 = chrono::high_resolution_clock::now();
	// gen_arr<int> a2(shapes[1], 9), b2(shapes[1], 2);
	// a2[2][0].getval() = 0;
	// // cout << "a2.shape" << vec_to_string(a2.get_shape()) << endl;
	// auto t_stamp22 = chrono::high_resolution_clock::now();
	// gen_arr<int> c2 = a2.transpose();
	// auto t_stamp23 = chrono::high_resolution_clock::now();
	// auto alloc_time221 = chrono::duration_cast<chrono::duration<double>>(t_stamp22 - t_stamp21).count();
	// auto alloc_time232 = chrono::duration_cast<chrono::duration<double>>(t_stamp23 - t_stamp22).count();

	// cout << "c2.shape" << vec_to_string(c2.get_shape()) << endl;
	// printf("Time(in seconds): Alloc:%f;\tComputation (tpose single):%f;\n", alloc_time221, alloc_time232);
	// // cout << "a's shape " << vec_to_string(a2.get_shape()) << endl;
	// // cout << "a2's dump: \n" << a2.str() << endl; // a2.dummy_dump()
	// // cout << "b2's dump: \n" << b2.str() << endl;
	// // cout << "c2's dump: \n" << c2.str() << endl;
	// auto t_stamp31 = chrono::high_resolution_clock::now();
	// gen_arr<int> a3(shapes[1], 9), b3(shapes[1], 2);
	// // a3[2][0].getval() = 0;
	// // cout << "a2.shape" << vec_to_string(a2.get_shape()) << endl;
	// auto t_stamp32 = chrono::high_resolution_clock::now();
	// gen_arr<int> c3 = a3.transpose_mt(num_thr);
	// auto t_stamp33 = chrono::high_resolution_clock::now();
	// auto alloc_time321 = chrono::duration_cast<chrono::duration<double>>(t_stamp32 - t_stamp31).count();
	// auto alloc_time332 = chrono::duration_cast<chrono::duration<double>>(t_stamp33 - t_stamp32).count();
	
	// cout << "c2.shape" << vec_to_string(c2.get_shape()) << endl;
	// printf("Time(in seconds): Alloc:%f;\tComputation (tpose multi):%f;\n", alloc_time321, alloc_time332);

	// gen_arr<int> bb0(shapes[2], 9);
	// vector<unsigned int> shape4 = bb0.get_shape();
	// reverse(shape4.begin(), shape4.end());
	// bb0[1][1].getval() = 0;
	// cout << "a3's dump: \n" << bb0.str() << endl;
	// bb0.reshape(shape4);
	// cout << "a3's dump after reshape: \n" << bb0.str() << endl;

	// // printf("Time(in seconds): Alloc:%f;\tComputation (single):%f;\tComputation(%d):%f;\n", alloc_time321, alloc_time332, num_thr, alloc_time332);
	


	return 0;
}