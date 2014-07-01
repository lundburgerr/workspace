
U = [1.72; 1.66; 0.99]-0.16;
p = [1.68E-8; 2.82E-8; 1.15E-7];


num = sum(log(U).*log(p))-sum(log(U))*sum(log(p).*log(p))/sum(log(p));
den = sum(log(p))-3*sum(log(p).*log(p))/sum(log(p));

c = num/den;
r = (3*c-sum(log(U)))/sum(log(p));

K = exp(c);

U.*p.^r
K

%% Resistivity for brass

Ub = 1.38-0.16;
pb = (K/Ub)^(1/r)


%% Resistivity assuming r=1/2
U.*p.^(-1/2)
C = sum(U.*p.^(-1/2))/3
pb = (Ub/C)^2