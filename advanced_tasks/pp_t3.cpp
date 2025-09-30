int main(int argc, char const *argv[])
{
    return 0;
}

doulbe x = 0;
for(int i = 0; i < n; i++)
{
    doulbe y = h;

    for(int j = 1; j < n; j++)
    {
        a =
    }
}
do i=0,n-1
y=h
    do j=1,n-1
    a(j)=-tau/h/h*s(x,y+h/2.)
    b(j)=1.+tau/h/h*(s(x,y+h/2.)+s(x,y-h/2.))
    c(j)=-tau/h/h*s(x,y-h/2.)
    d(j)=u1(i,j)+tau*f2(i,j)
    y=y+h
    enddo
!прямая прогонка
    !p(0)=0.;q(0)=u2(i,0)
    p(0)=1.;q(0)=0.
    do j=1,n-1
    p(j)=-c(j)/(b(j)+a(j)*p(j-1))
    q(j)=(d(j)-a(j)*q(j-1))/(b(j)+a(j)*p(j-1))
    enddo
!обратная прогонка
    do j=n-1,0,-1
    u2(i,j)=p(j)*u2(i,j+1)+q(j)
    enddo
x=x+h
enddo
k=k+1