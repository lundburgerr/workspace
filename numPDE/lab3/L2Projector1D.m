function L2Projector1D()

n = 5;
h = 1/n;
x = 0:h:1;
M = MassMat1D(x);
b = LoadVec(x, @foo);
Pf = M\b;