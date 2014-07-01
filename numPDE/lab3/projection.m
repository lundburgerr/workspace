close all;

N=6;
h = 1/N;
x = 0:h:1;

%% Compute the linear interpolant
f = @(x) x.*(1-x);
xi = 0:1/1000:1;
pif = zeros(1, length(xi));
for ii=1:N+1
    H = hatfun(xi, x, ii);
    pif = pif + f(x(ii))*H;
end

%% Compute the L2-projection
M = MassMat1D(x);
% M = [zeros(1,N+1); M; zeros(1,N+1)];
% M = [zeros(N+3,1), M, zeros(N+3,1)];
b = LoadVec1D(x, f);
coff = M\b;
Pf = zeros(1, length(xi));
for ii=2:N
    H = hatfun(xi, x, ii);
    Pf = Pf + coff(ii)*H;
end

plot(xi, pif, xi, Pf, xi, f(xi))