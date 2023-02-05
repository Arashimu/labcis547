int func(int a,int b){
    return a+b;
}
int main(){
    int a=1;
    int b=2;
    int *d=&a;
    int c=func(a,b);
    return 0;
}

