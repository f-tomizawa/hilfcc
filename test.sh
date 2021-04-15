#!/bin/bash
cat <<EOF | cc -xc -c -o tmp2.o -
int add(int x, int y) { return x+y; }
int sub(int x, int y) { return x-y; }
int add6(int a, int b, int c, int d, int e, int f) {
  return a+b+c+d+e+f;
}
EOF

assert() {
    expected="$1"
    input="$2"

    ./hilfcc "$input" > tmp.s
    cc -o tmp tmp.s tmp2.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 'main() { return 0; }'
assert 42 'main() { return 42; }'
assert 21 'main() { return 5+20-4; }'
assert 41 'main() { return  12 + 34 - 5 ; }'
assert 47 'main() { return 5+6*7; }'
assert 15 'main() { return 5*(9-6); }'
assert 4 'main() { return (3+5)/2; }'
assert 10 'main() { return -10+20; }'
assert 10 'main() { return - -10; }'
assert 10 'main() { return - - +10; }'

assert 0 'main() { return 0==1; }'
assert 1 'main() { return 42==42; }'
assert 1 'main() { return 0!=1; }'
assert 0 'main() { return 42!=42; }'

assert 1 'main() { return 0<1; }'
assert 0 'main() { return 1<1; }'
assert 0 'main() { return 2<1; }'
assert 1 'main() { return 0<=1; }'
assert 1 'main() { return 1<=1; }'
assert 0 'main() { return 2<=1; }'

assert 1 'main() { return 1>0; }'
assert 0 'main() { return 1>1; }'
assert 0 'main() { return 1>2; }'
assert 1 'main() { return 1>=0; }'
assert 1 'main() { return 1>=1; }'
assert 0 'main() { return 1>=2; }'

assert 3 'main() { a=3; return a; }'
assert 8 'main() { a=3; z=5; return a+z; }'

assert 3 'main() { a=3; return a; }'
assert 8 'main() { a=3; z=5; return a+z; }'
assert 6 'main() { a=b=3; return a+b; }'
assert 3 'main() { foo=3; return foo; }'
assert 8 'main() { foo123=3; bar=5; return foo123+bar; }'

assert 1 'main() { return 1; 2; 3; }'
assert 2 'main() { 1; return 2; 3; }'
assert 3 'main() { 1; 2; return 3; }'

assert 3 'main() { {1; {2;} return 3;} }'

assert 2 'main() { if (1 == 1) a = 2; return a; }'
assert 3 'main() { a = 3; if (1 != 1) a = 2; return a; }'
assert 2 'main() { if (4 == 2*2) return 2; return 3; }'
assert 2 'main() { if (2-1) return 2; return 3; }'
assert 4 'main() { if (0 == 1) return 3; else return 4; }'
assert 3 'main() { if (2 > 1) a = 3; else a = 4; return a; }'

assert 10 'main() { i=0; while(i<10) i=i+1; return i; }'

assert 55 'main() { sum=0; for (i=0; i<=10; i=i+1) sum=sum+i; return sum; }'
assert 45 'main() { i=0; sum=0; for (; i<10; i=i+1) sum=sum+i; return sum; }'
assert 3 'main() { for (;;) return 3; return 5; }'

assert 4 'main() { a=3; b=4; if (a < b) {c=a; a=b; b=c;} return a; }'
assert 45 'main() { sum=0; i=0; while(i<10) {sum = sum + i; i = i + 1;} return sum; }'
assert 135 'main() { ans=1; sum=0; for (i=1; i<=5; i=i+1) { ans = ans * i; sum = sum + i;} return ans + sum; }'

assert 3 'main() { a=ret3(); return a; } ret3() { return 3; }'
assert 15 'main() { return ret3() * ret5(); } ret3() { return 3; } ret5() { return 5; } '

assert 8 'main() { return add(3, 5); }'
assert 2 'main() { return sub(5, 3); }'
assert 21 'main() { return add6(1,2,3,4,5,6); }'
assert 66 'main() { return add6(1,2,add6(3,4,5,6,7,8),9,10,11); }'
assert 136 'main() { return add6(1,2,add6(3,add6(4,5,6,7,8,9),10,11,12,13),14,15,16); }'

echo OK