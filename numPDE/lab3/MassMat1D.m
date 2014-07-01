function M = MassMat1D(x)
n = length(x)-1;
M = zeros(n+1, n+1);
for ii = 1:n
    h = x(ii+1)-x(ii);
    M(ii,ii) = M(ii,ii) + h/3;
    M(ii,ii+1) = M(ii,ii+1) + h/6;
    M(ii+1,ii) = M(ii+1,ii) + h/6;
    M(ii+1,ii+1) = M(ii+1,ii+1) + h/3;
end