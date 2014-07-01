function u = fftimm(dx, f, periodic)

global L;
global c;

x = 0:dx:L;
N = length(x);
fs = 1/dx;
k = zeros(N, 1);
k(1) = 0;
for ii=2:N/2+1
    k(ii) = (ii-1)*fs/N;
    k(N+2-ii) = k(ii);
end
fx = [0;f(x(2:end-1))';0];

%Make periodic
if( ~strcmp(periodic, 'periodic'))    
    k = [k; -k];
    fx = [fx; -fx];
end

A = fft(fx); %TODO: Should I put ends zero here?

if( ~strcmp(periodic, 'periodic'))    
    U = @(t) A.*cos(2*pi*k*c*t); %TODO: Why do I need this factor of 0.5, has something todo with twice the size of some variables but how??
else
    U = @(t) A.*cos(2*pi*k*c*t); %TODO: Why do I need this factor of 0.5, has something todo with twice the size of some variables but how??
end


u = @(t) ifft(U(t), 'symmetric');