function jac = bzJac(t,y)
jac = zeros(3,3);
global a
global b
global c
% Definition of the jacobian for the ODE
% y(1) = ?
% y(2) = ?
% y(3) = ?
jac(1,1) = a*(1-2*b*y(1)-y(2));
jac(1,2) = a*(1-y(1));
jac(1,3) = 0;

jac(2,1) = -y(2)/a;
jac(2,2) = -(1+y(1))/a;
jac(2,3) = 1/a;

jac(3,1) = c;
jac(3,2) = 0;
jac(3,3) = -c;
%--- End of code ---