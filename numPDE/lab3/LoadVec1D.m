function b = LoadVec1D(x,f)
n = length(x)-1;
b = zeros(n+1,1);
for ii=1:n
    h = x(ii+1)-x(ii);
    b(ii) = b(ii) + f(x(ii))*h/2;
    b(ii+1) = b(ii+1) + f(x(ii+1))*h/2;
end
% for ii=2:n
%     h = x(ii+1)-x(ii-1);
%     h = h/2;
%     b(ii) = b(ii) + h/6*f(x(ii-1));
%     b(ii) = b(ii) + h/6*4*f(x(ii));
%     b(ii) = b(ii) + h/6*f(x(ii+1));
% end
% for ii=2:n
%     h = x(ii+1)-x(ii);
%     x0 = (x(ii)+x(ii-1))/2;
%     x1 = x(ii);
%     x2 = (x(ii+1)+x(ii))/2;
%     b(ii) = b(ii) + h/6*f(x0);
%     b(ii) = b(ii) + h/6*4*f(x1);
%     b(ii) = b(ii) + h/6*f(x2);
% end