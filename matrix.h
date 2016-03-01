/* matrix.h

   Flexible template-based implementation of matrices and vectors.
   (You may want to consult the attached matrix_test.cpp file for examples of
    the use of the data structures in this file rather than trying to read
	the code).
   
   B. Bird - 05/26/2010 (Updated 02/08/2016)
*/

#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>

//A generalized m row, n column matrix
//Stored in row major order
template<int m, int n> 
class GeneralMatrix{
public:
	typedef float EntryType;
	
	static const int numRows = m;
	static const int numCols = n;

	GeneralMatrix(){
		setzero();
	}
	//Create a matrix from a (row-major) array
	GeneralMatrix(EntryType Entries[m][n]){
		int i,j;
		for (i = 0; i < m; i++)
			for(j = 0; j < n; j++)
				Entry(i,j) = Entries[i][j];
	}
	
	//Create a matrix from a pointer to a (row or column major) array with stride bytes between the beginning of each row (or column if column major is used)
	GeneralMatrix(EntryType *Entries,int stride,bool rowMajor = true){
		int i,j;
		for (i = 0; i < m; i++)
			for(j = 0; j < n; j++)
				Entry(i,j) = rowMajor? Entries[i*stride+j]: Entries[j*stride+i];
	}
	
	//Copy Constructors
	GeneralMatrix(const GeneralMatrix<m,n> &other){ 
		int i,j;
		for (i = 0; i < m; i++)
			for( j = 0; j < n; j++)
				Entry(i,j) = other.GetEntry(i,j);		
	}
	GeneralMatrix<m,n>& operator=(const GeneralMatrix<m,n>& other){
		int i,j;
		for (i = 0; i < m; i++)
			for( j = 0; j < n; j++)
				Entry(i,j) = other.GetEntry(i,j);	
		return *this;
	}
	
	void setzero(){
		int i,j;
		for (i = 0; i < m; i++)
			for (j = 0; j < n; j++)
				Entry(i,j) = 0;
	}
	
	EntryType& operator()(int row,int col){
		return entries[row][col];
	}
	EntryType& Entry(int row,int col){
		return (*this)(row,col);
	}
	EntryType GetEntry(int row,int col) const{
		return (*const_cast<GeneralMatrix<m,n>* >(this))(row,col);
	}
	
	GeneralMatrix<m,n> operator * (EntryType s) const{ //Multiply by scalar
		GeneralMatrix<m,n> ret;
		int i,j;
		for (i = 0; i < m; i++)
			for (j = 0; j < n; j++)
				ret(i,j) = this->GetEntry(i,j)*s;
		return ret;
	}
	GeneralMatrix<m,n>& operator *= (EntryType s){ //Multiply by scalar (in place)
		int i,j;
		for (i = 0; i < m; i++)
			for (j = 0; j < n; j++)
				Entry(i,j) *= s;
		return *this;
	}
	
	
	template<class T>
	T operator + (const T& mat2) const{ //Add Matrix
		T ret;
		int i,j;
		for (i = 0; i < m; i++)
			for (j = 0; j < n; j++)
				ret.Entry(i,j) += this->GetEntry(i,j)+mat2.GetEntry(i,j);
		return ret;
	}
	
	GeneralMatrix<m,n>& operator += (GeneralMatrix<m,n>& other){ //Add Matrix (in place)
		int i,j;
		for (i = 0; i < m; i++)
			for (j = 0; j < n; j++)
				entries[i][j] *= other.entries[i][j];
		return *this;
	}
	template<int p>
	GeneralMatrix<m,p> operator * (const GeneralMatrix<n,p>& mat2) const{ //Multiply
		GeneralMatrix<m,p> ret;
		MatMult(*this,mat2,ret);
		return ret;
	}
	template<int p>
	static void MatMult(const GeneralMatrix<m,n>& mat1, const GeneralMatrix<n,p>& mat2, GeneralMatrix<m,p>& out){
		int i,j,k;
		for (i = 0; i < m; i++)
			for (j = 0; j < p; j++){
				EntryType sum = 0;
				for (k = 0; k < n; k++)
					sum += mat1.GetEntry(i,k)*mat2.GetEntry(k,j);
				out(i,j) = sum;
			}
	}
	
	void print(){
		int i,j;
		for (i = 0; i < m; i++){
			for (j = 0; j < n; j++)
				printf("%.2f ",Entry(i,j));
			printf("\n");
		}
		printf("\n");
	}


protected:
	EntryType entries[m][n];
};


template<int n>
class Vector: public GeneralMatrix<n,1>{
public:
	typedef typename GeneralMatrix<n,1>::EntryType EntryType;
	Vector(): GeneralMatrix<n,1>(){}
	Vector(EntryType Entries[n]): GeneralMatrix<n,1>( Entries,1,true ){}
	EntryType DotProduct(const Vector<n>& other) const{
		EntryType sum = 0;
		int i;
		for (i = 0; i < n; i++){
			sum+= this->GetEntry(i,0)*other.GetEntry(i,0);
		}
		return sum;
	}
	EntryType& operator()(int i){
		return this->Entry(i,0);
	}

};


template <int n>
class SquareMatrix: public GeneralMatrix<n,n>{
public:
	typedef typename GeneralMatrix<n,n>::EntryType EntryType;
	void T(){
		int i,j;
		EntryType swap;
		for (i = 0; i < n; i++)
			for (j = i+1; j < n; j++){
				swap = (*this)(j,i);
				(*this)(j,i) = (*this)(i,j);
				(*this)(i,j) = swap;
			}
	}
	void identity(){
		int i,j;
		for (i = 0; i < n; i++)
			for( j = 0; j < n; j++)
				(*this)(i,j) = (i==j)?1:0;
	}
	
	//We want Left Multiplication to produce an output identical to it's input
	//(i.e. the thing on the right)
	//That might be a matrix or an n x n vector, so we have to wave our hands and just
	//hope a template can figure that out...
	//Also, we use 'U' instead of 'T' for the template variable because T is a method name.
	template<class U>
	static inline void LeftMultiply(const SquareMatrix<n>& left, const U& right, U& out){
		GeneralMatrix<n,n>::MatMult(left,right,out);
	}
	template<class U>
	static inline void LeftMultiply(const SquareMatrix<n>& left, const U& right, SquareMatrix<n>& out){
		GeneralMatrix<n,n>::MatMult(left,right,out);
	}
	template<class U>
	U operator *(const U &right) const{
		U ret;
		LeftMultiply(*this,right,ret);
		return ret;
	}
	
	//Since these are square matrices, we can define in-place multiplication
	template<class U>
	SquareMatrix<n>& operator *=(const U &right){
		SquareMatrix<n> tmp;
		LeftMultiply(*this,right,tmp);
		*this = tmp;
		return *this;
	}
};

class Matrix3: public SquareMatrix<3>{
public:
	typedef typename SquareMatrix<3>::EntryType EntryType;
	Matrix3(): SquareMatrix<3>(){};
	Matrix3(EntryType a00, EntryType a01, EntryType a02, 
		    EntryType a10, EntryType a11, EntryType a12,
			EntryType a20, EntryType a21, EntryType a22):
				SquareMatrix<3>()
			{
		this->Entry(0,0) = a00;
		this->Entry(0,1) = a01;
		this->Entry(0,2) = a02;
		this->Entry(1,0) = a10;
		this->Entry(1,1) = a11;
		this->Entry(1,2) = a12;
		this->Entry(2,0) = a20;
		this->Entry(2,1) = a21;
		this->Entry(2,2) = a22;
	}
};

class Matrix2: public SquareMatrix<2>{
public:
	typedef typename SquareMatrix<2>::EntryType EntryType;
	Matrix2(): SquareMatrix<2>(){};
	Matrix2(EntryType a00, EntryType a01,
		    EntryType a10, EntryType a11):
				SquareMatrix<2>()
			{
		this->Entry(0,0) = a00;
		this->Entry(0,1) = a01;
		this->Entry(1,0) = a10;
		this->Entry(1,1) = a11;
	}
	Matrix3 ToHomogeneousMatrix3() const{
		return Matrix3( this->GetEntry(0,0), this->GetEntry(0,1), 0,
						this->GetEntry(1,0), this->GetEntry(1,1), 0,
						0, 0, 1);
	}
};

class Vector3: public Vector<3>{
public:
	typedef typename Vector<3>::EntryType EntryType;
	Vector3(): Vector<3>(), x(entries[0][0]), y(entries[1][0]), z(entries[2][0]) {}
	Vector3(EntryType e[3]): Vector<3>(e), x(entries[0][0]), y(entries[1][0]), z(entries[2][0]){}
	Vector3(EntryType x, EntryType y, EntryType z):  x(entries[0][0]), y(entries[1][0]), z(entries[2][0]){
		this->Entry(0,0) = x;
		this->Entry(1,0) = y;
		this->Entry(2,0) = z;
	}
	Vector3(const GeneralMatrix<3,1>& other): x(entries[0][0]), y(entries[1][0]), z(entries[2][0]){
		this->Entry(0,0) = other.GetEntry(0,0);
		this->Entry(1,0) = other.GetEntry(1,0);
		this->Entry(2,0) = other.GetEntry(2,0);
	}
	Vector3& operator=(const GeneralMatrix<3,1>& other){
		*((GeneralMatrix<3,1>*)this) = other;
		return *this;
	}
	Vector3& operator=(const Vector3& other){
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}
	static void CrossProduct(Vector3 &v1, Vector3 &v2, Vector3& out){
		out(0) = v1(1)*v2(2) - v1(2)*v2(1);
		out(1) = v1(2)*v2(0) - v1(0)*v2(2);
		out(2) = v1(0)*v2(1) - v1(1)*v2(0);
	}
	Vector3 CrossProduct(Vector3& other){
		Vector3 ret;
		CrossProduct(*this,other,ret);
		return ret;
	}
	EntryType& x;
	EntryType& y;
	EntryType& z;
};

class Vector2: public Vector<2>{
public:
	typedef typename Vector<2>::EntryType EntryType;
	Vector2(): Vector<2>(), x(entries[0][0]), y(entries[1][0]) {};
	Vector2(EntryType e[2]): Vector<2>(e), x(entries[0][0]), y(entries[1][0]){};
	Vector2(EntryType x, EntryType y): x(entries[0][0]), y(entries[1][0]){
		this->Entry(0,0) = x;
		this->Entry(1,0) = y;
	}
	Vector2(const GeneralMatrix<2,1>& other): x(entries[0][0]), y(entries[1][0]){
		this->Entry(0,0) = other.GetEntry(0,0);
		this->Entry(1,0) = other.GetEntry(1,0);
	}
	Vector2& operator=(const GeneralMatrix<2,1>& other){
		*((GeneralMatrix<2,1>*)this) = other;
		return *this;
	}
	Vector2& operator=(const Vector2& other){
		x = other.x;
		y = other.y;
		return *this;
	}
	Vector3 ToHomogeneousVector3() const{
		return Vector3(this->GetEntry(0,0), this->GetEntry(0,1), 1);
	}
	EntryType& x;
	EntryType& y;
};



#endif
