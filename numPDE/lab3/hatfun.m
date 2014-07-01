function H = hatfun(xi,x,k)
% HATFUN Evaluate a hat function (PL basis function).
% Input: 
%   xi  evaluation points (vector)
%   x   interpolation nodes (vector, length n+1)
%   k   node index (integer, in [1,n+1])
% Output:
%   H   values of the kth basis function

n = length(x)-1;
% "Fictitious nodes" to deal with first, last funcs.
x = [ 2*x(1)-x(2); x(:); 2*x(n+1)-x(n) ];
k = k+1;  % adjust index

H1 = max( 0, (xi-x(k-1))/(x(k)-x(k-1)) ); % upward slope
H2 = max( 0, (x(k+1)-xi)/(x(k+1)-x(k)) ); % downward slope
H = min(H1,H2);
