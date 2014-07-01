function u = crank(dx, dt, f)

global L;
global T;
global c;

x = 0:dx:L;
t = 0:dt:T;

N = length(x)-2;
u = f(x(2:end-1))';
uold = u;

delta = (c*dt/dx)^2;
M = (1+delta)*diag(ones(N,1)) - 0.5*delta*diag(ones(N-1,1), 1) - 0.5*delta*diag(ones(N-1,1), -1);
Minv = inv(M);


for ii=1:length(t)
    tmp = u;    
    u = Minv*(2*u - M*uold);
 
    uold = tmp;
    
    figure(1); hold off;
    plot(x, [0;u;0]); ylim([-1 1]);
    pause(0.002);
end

u = [0;u;0];