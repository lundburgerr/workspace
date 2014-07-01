function u = ffttime(dx, dt, f)

global L;
global T;
global c;

x = 0:dx:L;
t = 0:dt:T;

N = length(x);
u = f(x(2:end-1))';
uold = u;

fs = 1/dx;
k = zeros(N, 1);
k(1) = 0;
for ii=2:N/2+1
    k(ii) = (ii-1)*fs/N;
    k(N+2-ii) = k(ii);
end
% k = 2*pi*k;
k2 = k.*k;

for ii=1:length(t)
    tmp = u;
    fftmp = ifft(k2.*fft([0;u;0]), 'symmetric');
    u = 2*u-uold - (c*dt)^2*fftmp(2:end-1);
 
    uold = tmp;
    
    figure(1); hold off;
    plot(x, [0;u;0]); ylim([-1 1]);
    pause(0.0004);
end

u = [0;u;0];