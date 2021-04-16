#!/bin/bash

assert() {
    expected="$1"
    input="$2"

    ./hilfcc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 'int main() { return 0; }'
assert 42 'int main() { return 42; }'
assert 21 'int main() { return 5+20-4; }'
assert 41 'int main() { return  12 + 34 - 5 ; }'
assert 47 'int main() { return 5+6*7; }'
assert 15 'int main() { return 5*(9-6); }'
assert 4 'int main() { return (3+5)/2; }'
assert 10 'int main() { return -10+20; }'
assert 10 'int main() { return - -10; }'
assert 10 'int main() { return - - +10; }'

assert 0 'int main() { return 0==1; }'
assert 1 'int main() { return 42==42; }'
assert 1 'int main() { return 0!=1; }'
assert 0 'int main() { return 42!=42; }'

assert 1 'int main() { return 0<1; }'
assert 0 'int main() { return 1<1; }'
assert 0 'int main() { return 2<1; }'
assert 1 'int main() { return 0<=1; }'
assert 1 'int main() { return 1<=1; }'
assert 0 'int main() { return 2<=1; }'

assert 1 'int main() { return 1>0; }'
assert 0 'int main() { return 1>1; }'
assert 0 'int main() { return 1>2; }'
assert 1 'int main() { return 1>=0; }'
assert 1 'int main() { return 1>=1; }'
assert 0 'int main() { return 1>=2; }'

assert 3 'int main() { int a; a=3; return a; }'
assert 8 'int main() { int a; int z; a=3; z=5; return a+z; }'

assert 6 'int main() { int a; int b; a=b=3; return a+b; }'
assert 3 'int main() { int foo; foo=3; return foo; }'
assert 8 'int main() { int foo123; int bar; foo123=3; bar=5; return foo123+bar; }'

assert 1 'int main() { return 1; 2; 3; }'
assert 2 'int main() { 1; return 2; 3; }'
assert 3 'int main() { 1; 2; return 3; }'

assert 3 'int main() { {1; {2;} return 3;} }'

assert 2 'int main() { int a; if (1 == 1) a = 2; return a; }'
assert 3 'int main() { int a; a = 3; if (1 != 1) a = 2; return a; }'
assert 2 'int main() { if (4 == 2*2) return 2; return 3; }'
assert 2 'int main() { if (2-1) return 2; return 3; }'
assert 4 'int main() { if (0 == 1) return 3; else return 4; }'
assert 3 'int main() { int a; if (2 > 1) a = 3; else a = 4; return a; }'

assert 10 'int main() { int i; i=0; while(i<10) i=i+1; return i; }'

assert 55 'int main() { int sum; int i; sum=0; for (i=0; i<=10; i=i+1) sum=sum+i; return sum; }'
assert 45 'int main() { int sum; int i; i=0; sum=0; for (; i<10; i=i+1) sum=sum+i; return sum; }'
assert 3 'int main() { for (;;) return 3; return 5; }'

assert 4 'int main() { int a; int b; a=3; b=4; if (a < b) {int c; c=a; a=b; b=c;} return a; }'
assert 45 'int main() { int sum; int i; sum=0; i=0; while(i<10) {sum = sum + i; i = i + 1;} return sum; }'
assert 135 'int main() { int ans; int sum; int i; ans=1; sum=0; for (i=1; i<=5; i=i+1) { ans = ans * i; sum = sum + i;} return ans + sum; }'

assert 3 'int main() { int a; a=ret3(); return a; } int ret3() { return 3; }'
assert 15 'int main() { return ret3() * ret5(); } int ret3() { return 3; } int ret5() { return 5; } '

assert 8 'int main() { return add(3, 5); } int add(int x, int y) { return x+y; }'
assert 2 'int main() { return sub(5, 3); } int sub(int x, int y) { return x-y; }'
assert 21 'int main() { return add6(1,2,3,4,5,6); } int add6(int a, int b, int c, int d, int e, int f) { return a+b+c+d+e+f; }'
assert 66 'int main() { return add6(1,2,add6(3,4,5,6,7,8),9,10,11); } int add6(int a, int b, int c, int d, int e, int f) { return a+b+c+d+e+f; }'
assert 136 'int main() { return add6(1,2,add6(3,add6(4,5,6,7,8,9),10,11,12,13),14,15,16); } int add6(int a, int b, int c, int d, int e, int f) { return a+b+c+d+e+f; }'

assert 7 'int main() { return add2(3,4); } int add2(int x, int y) { return x+y; }'
assert 1 'int main() { return sub2(4,3); } int sub2(int x, int y) { return x-y; }'
assert 55 'int main() { return fib(9); } int fib(int x) { if (x<=1) return 1; return fib(x-1) + fib(x-2); }'

assert 3 'int main() { int x; x=3; return *&x; }'
assert 3 'int main() { int x; int y; int z; x=3; y=&x; z=&y; return **z; }'
assert 5 'int main() { int x; int y; x=3; y=5; return *(&x+8); }'
assert 3 'int main() { int x; int y; x=3; y=5; return *(&y-8); }'

echo OK