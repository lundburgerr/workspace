close all;

N=6;
h = 1/N;
x = 0:h:1;
xi = 0:1/1000:1;

%% Compute the solution for -u''=2
f = @(x) 2;
M = MassMat1D(x);
% M = [zeros(1,N+1); M; zeros(1,N+1)];
% M = [zeros(N+3,1), M, zeros(N+3,1)];
b = LoadVec1D(x, f);
b = ones(N+1,1)*2;
coff = M\b;
u = zeros(1, length(xi));
for ii=1:N+1
    H = hatfun(xi, x, ii);
    u = u + coff(ii)*H;
end

plot(xi, u)
