close all;

%% TODO: Fix \phi so that it displays phi in plots

%% exc 1.12
h = 0.1;
relTol = eps;
x = -1:h:1;
y = -1:h:1;


rows = length(x);
cols = length(y);
N = rows*cols;

phi = 0.1*ones(rows*cols,1);

%Insert boundary values
phi(1:cols) = 0; %Bottom
phi(rows*(cols-1):rows*cols) = 0; %top
phi(1:cols:rows*(cols-1)) = 0; %left
phi(rows:cols:rows*cols) = 0; %right

%Insert value at phi(0,0)
%TODO: This only works for odd rows*cols.
phi(ceil(rows*cols/2)) = -1;

%Create transformation matrix
T = eye(rows*cols);

for n=1:(rows-2)
    for m=1:(cols-2)
        v = zeros(1, cols*rows);
        v(n*cols+1+m + 1) = 1;
        v(n*cols+1+m - 1) = 1;
        v(n*cols+1+m + cols) = 1;
        v(n*cols+1+m - cols) = 1;
        T(n*cols+1+m, :) = 0.25*v;
    end
end
v = zeros(1, cols*rows);
v(ceil(rows*cols/2)) = 1;
T45 = T;
T(ceil(rows*cols/2), :) = v;

%Solve problem
line = zeros(ceil(cols/2), 3); %Stores phi(0:1,0) for different iterations
line(:,1) = x(ceil(cols/2):end);
phiOld = zeros(rows*cols, 1);
n = 0;
while true
    n = n+1;
    phiOld = phi;
    phi = T*phi;
    e = norm(phi-phiOld)/max(abs(phi));
    if e < relTol
        fprintf('Iterate to: %d; e=%g\n',n,e);
        break;
    end
    if n == 100
        line(:, 2) = phi(ceil(N/2):ceil(N/2)+ceil(cols/2)-1);
    end
    if n == 1000
        line(:, 3) = phi(ceil(N/2):ceil(N/2)+ceil(cols/2)-1);
    end
end

line
Z = zeros(rows, cols);
for n=1:rows
    Z(rows+1-n,:) = phi((n-1)*cols+1:n*cols)';
end

handle = figure;
surf(x, y, Z);
xlabel('x'); ylabel('y'); zlabel('\phi');
saveas(handle, 'report/img/exc1_laplace.png');

%% exc1.3 Calculate E
E = zeros(rows, cols);

for n=2:cols-1
    for m=2:rows-1
        Ex = -(Z(n+1, m)-Z(n-1, m))/(2*h);
        Ey = -(Z(n, m+1)-Z(n, m-1))/(2*h);
        E(n,m) = sqrt(Ex^2+Ey^2);
    end
end

%plot
handle = figure;
plot(h:h:1-h, E(ceil(rows/2), ceil(cols/2)+1:cols-1));
xlabel('x'); ylabel('|E|');
saveas(handle, 'report/img/exc1_E.png')

handle = figure;
loglog(1./(h:h:1-h), E(ceil(rows/2), ceil(cols/2)+1:cols-1));
xlabel('log(1/x)'); ylabel('log(|E|)');
saveas(handle, 'report/img/exc1_logE.png');

%% exc1.45 Poissons equation.
phi = 0.1*ones(rows*cols,1);

%Insert boundary values
phi(1:cols) = 0; %Bottom
phi(rows*(cols-1):rows*cols) = 0; %top
phi(1:cols:rows*(cols-1)) = 0; %left
phi(rows:cols:rows*cols) = 0; %right

%Insert value at phi(0,0)
%TODO: This only works for odd rows*cols.
phi(ceil(rows*cols/2)) = -1;

rho = zeros(cols*rows, 1);
rho(ceil(cols*rows/2)) = pi;

n = 0;
while true
    n = n+1;
    phiOld = phi;
    phi = T45*phi + rho;
    e = norm(phi-phiOld)/max(abs(phi));
    if e < relTol
        fprintf('Iterate to: %d; e=%g\n',n,e);
        break;
    end
    if n == 100
        line(:, 2) = phi(ceil(N/2):ceil(N/2)+ceil(cols/2)-1);
    end
    if n == 1000
        line(:, 3) = phi(ceil(N/2):ceil(N/2)+ceil(cols/2)-1);
    end
end

Z = zeros(rows, cols);
for n=1:rows
    Z(rows+1-n,:) = phi((n-1)*cols+1:n*cols)';
end

handle = figure; 
surf(x, y, Z);
xlabel('x'); ylabel('y'); zlabel('\phi');
saveas(handle, 'report/img/exc1_poisson.png');

