a = 9;
B = 503;
A = 23.5;
D = 660;

sa = 1.5;
sB = 3;
sA = 2.5;
sD = 3;

d = (a*B-A*(D-B))/D;

sd = (B/D*sa)^2 + ((B/D-1)*sA)^2 + ((a/D+A/D)*sB)^2 + ((a*B/D^2+A*B/D^2)*sD)^2;
sd = sqrt(sd);

disp(sprintf('d = %f +- %f mm', d, sd));