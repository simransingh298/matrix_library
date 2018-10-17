#include <iostream>
#include <list>
#include <memory>
// #include "myMat.cpp"

using namespace std;

string list_int_to_string(list<int> l){
    string res = "[";
    for (auto val : l){
        res += to_string(val) + ",";
    }
    res += "]";
    return res;
}
int prod_elems_in_list(list<int> &l){
    int res = 1;
    for(int elem : l){
        res *= elem;
    }
    return res;
}
class generic_array{
private:
	unique_ptr<generic_array[]> arr;
	int ga_length;
	int val;
	int ndim;
	list<int> shape;
	void init_helper(list<int>shape){
		this->shape = shape;
		this->ndim = shape.size();
		if (this->ndim == 0){
			this->ga_length = 0;
            this->arr.reset();
        }
		else{
			this->ga_length = shape.front();
			this->arr = unique_ptr<generic_array[]>(new generic_array[this->ga_length]);
		}
	}
	string str_helper(string append){
		if (this->ndim == 0){
			return to_string(this->val);
		}
		else if (this->ndim == 1){
			string res = "";
			res += "[";
			for (int i = 0; i < this->ga_length; i++){
				res += this->arr[i].str_helper("");
                res += ",";
			}
			res[res.length()-1] = ']';
			return append+res;
		}
		else{
			string res = append + "[\n";
			string child_append = append + "\t";
			for (int i = 0; i < this->ga_length; i++){
				res += this->arr[i].str_helper(child_append)+";\n";
			}
			res += append+"]";
			return res;
		}
	}
public:
	void init(list<int>shape){
		init_helper(shape);
		this->val = 0;
		if (this->ndim != 0){
			shape.pop_front();
			for (int i = 0; i < this->ga_length; i++){
				this->arr[i].init(shape);
			}
		}
	}
	void init(list<int> shape, int val){
		init_helper(shape);
		this->val = val;
		if (this->ndim != 0){
			shape.pop_front();
			for (int i = 0; i < this->ga_length; i++){
				this->arr[i].init(shape, val);
			}
		}
	}
    void init(list<int> shape, int* flat_arr){
        init_helper(shape);
        if (this->ndim == 0){
            this->ga_length = 0;
            this->arr.reset();
            this->val = flat_arr[0];
        }
        else{
            shape.pop_front();
            this->val = 0;
            int num_elem_in_submats = prod_elems_in_list(shape);
            for(int i = 0; i < this->ga_length; i++){
                this->arr[i].init(shape, &(flat_arr[num_elem_in_submats*i]));
            }
        }
    }
	generic_array(){
		this->arr.reset();
		this->val = 0;
		this->ndim = 0;
		this->ga_length = 0;
	}
	generic_array(list<int> shape){
		init(shape);
	}
	generic_array(list<int> shape, int init_val){
		init(shape, init_val);
	}
    generic_array(list<int>shape, int* flat_arr){
        init(shape, flat_arr);
    }
    generic_array(const generic_array &src){
        // cout << "copy constructor called. shape = " << list_int_to_string(src.shape) << "\n";
        this->ga_length = src.ga_length;
        this->ndim = src.ndim;
        this->val = src.val;
        this->shape = src.shape;
        if (this->ndim == 0){
            this->arr.reset();
        }
        else{
			this->arr = unique_ptr<generic_array[]>(new generic_array[this->ga_length]);
            for (int i = 0; i < this->ga_length; i++){
                this->arr[i] = src.arr[i];
            }
        }
    }
    ~generic_array(){
        this->arr.reset();
    }
public:
	string str(){
		return str_helper("");
	}
    list<int> get_shape(){
        return this->shape;
    }
    generic_array add(generic_array const &rhs){
        generic_array res = *this;
        if (rhs.shape == this->shape){
            res.val += rhs.val;
            if (this->ndim != 0){
                for (int i = 0; i < this->ga_length; i++){
                    res.arr[i] = this->arr[i].add(rhs.arr[i]);
                }
            }
        }
        else{
            cout << "Incompatible shapes\n";
        }
        return res;
    }
    generic_array sub(generic_array const &rhs){
        generic_array res = *this;
        if (rhs.shape == this->shape){
            res.val -= rhs.val;
            if (this->ndim != 0){
                for (int i = 0; i < this->ga_length; i++){
                    res.arr[i] = this->arr[i].sub(rhs.arr[i]);
                }
            }
        }
        else{
            cout << "Incompatible shapes\n";
        }
        return res;
    }
    generic_array elem_mul(generic_array const & rhs){
        generic_array res = *this;
        if (rhs.shape == this->shape){
            res.val *= rhs.val;
            if (this->ndim != 0){
                for (int i = 0; i < this->ga_length; i++){
                    res.arr[i] = this->arr[i].elem_mul(rhs.arr[i]);
                }
            }
        }
        else{
            cout << "Incompatible shapes\n";
        }
        return res;
    }
    generic_array elem_div(generic_array const & rhs){
        generic_array res = *this;
        if (rhs.shape == this->shape){
            res.val /= rhs.val;
            if (this->ndim != 0){
                for (int i = 0; i < this->ga_length; i++){
                    res.arr[i] = this->arr[i].elem_div(rhs.arr[i]);
                }
            }
        }
        else{
            cout << "Incompatible shapes\n";
        }
        return res;
    }
    generic_array & operator=(generic_array const &rhs){
        this->ga_length = rhs.ga_length;
        this->ndim = rhs.ndim;
        this->val = rhs.val;
        this->shape = rhs.shape;
        if (this->ndim == 0){
            this->arr.reset();
        }
        else{
			this->arr = unique_ptr<generic_array[]>(new generic_array[this->ga_length]);
            for (int i = 0; i < this->ga_length; i++){
                this->arr[i] = rhs.arr[i];
            }
        }
    }
    generic_array & operator+=(generic_array const &rhs){
        if (rhs.shape == this->shape){
            this->val += rhs.val;
            if (this->ndim != 0){
                for (int i = 0; i < this->ga_length; i++){
                    this->arr.get()[i] += rhs.arr.get()[i];
                }
            }
        }
        else{
            cout << "Incompatible shapes\n";
        }
        return *this;
    }
    generic_array operator+(generic_array const &rhs){
        return this->add(rhs);
    }
    generic_array operator-(generic_array const &rhs){
        return this->sub(rhs);
    }
    generic_array operator*(generic_array const &rhs){
        return this->elem_mul(rhs);
    }
    generic_array operator/(generic_array const & rhs){
        return this->elem_div(rhs);
    }
    generic_array & operator+(){
        return *this;
    }
    generic_array operator-(){
        generic_array res = *this;
        res.val = -(this->val);
        if (this->ndim != 0){
            for (int i = 0; i < this->ga_length; i++){
                res.arr.get()[i] = -(this->arr.get()[i]);
            }
        }

        return res;
    }
    
};

generic_array arange(list<int> &shape, int start, int stop, int step){
    if ((start - stop > 0 && step > 0) || (stop - start > 0 && step < 0) || start - stop == 0){
        return generic_array(shape);
    }
    int num_elems = (stop+1-start) / step;
    num_elems = num_elems > 0 ? num_elems : -num_elems;

    unique_ptr<int[]> buf(new int[num_elems]);
    for (int i = 0; i < num_elems; i++){
        buf[i] = start + step * i;
    }
    return generic_array(shape, buf.get());
}

generic_array arange(list<int> &shape, int stop){
    return arange(shape, 0, stop, stop > 0 ? 1 : -1);
}

generic_array arange(list<int> &shape, int start, int stop){
    return arange(shape, start, stop, stop > start ? 1 : -1);
}

int main(){
    int shapearr[] = {4, 5};
    int valarr[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
    list<int> shape(shapearr, shapearr+2);
    generic_array ga(shape, 43), gb(shape, 3);
    cout << "#############\n";
    generic_array gc(shape), ge(shape), gf(shape);
    gc = ga + -gb;
    ga += gc;
    generic_array gd = ga - gc;
    ge = ga * gb;
    gf = ga / gb;
    cout << ga.str() << endl << gb.str() << endl << gc.str() << endl << gd.str() << endl << ge.str() << endl << gf.str() << endl;
    generic_array gg(shape, valarr);
    cout << gg.str() << endl;
    cout << arange(shape, 3, 44, 2).str();
    cout << arange(shape, 59, 21).str();
    return 0;
}
