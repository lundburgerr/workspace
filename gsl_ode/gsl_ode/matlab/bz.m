function dy = bz(t,y)
dy = zeros(3,1);
global a
global b
global c
% Definition of the ODE
% y(1) = ?
% y(2) = ?
% y(3) = ?
dy(1) = a*(y(2)+y(1)*(1-b*y(1)-y(2)));
dy(2) = 1/a*(y(3)-(1+y(1))*y(2));
dy(3) = c*(y(1)-y(3));
%--- End of code ---