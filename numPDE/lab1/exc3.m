close all;
C = {'k','b','r',[0.1, 0.7, 0.1],[0.7, 0, 0.5], 'm'};
%%exc 3
%Initial values
vm = 25;
pm = 1;

NG = 40;
L = 400;
dx = L/NG;
dt = dx/vm; %TODO: choose better value for dt.
x = dx/2:dx:L-dx/2;

pu = zeros(NG, 1);
pd = zeros(NG, 1);

eta = 0.1;
p = eta*pm*exp( -((x-L/4)/(L/8)).^2 )';
hp = figure; hold on;
hv = figure; hold on;
n = 1;
for nLW=1:31
    pu = [p(2:end); p(1)];
    pd = [p(end); p(1:end-1)];
    %TODO: All these F's just become zero
    Fu = pu*vm.*(1-pu/pm);
    Fd = pd*vm.*(1-pd/pm);
    v  = vm*(1-p/pm);
    F = p.*v;
    qu = vm*(1-(pu+p)/pm);
    qd = vm*(1-(p+pd)/pm);
        
    if mod(nLW-1, 6) == 0
        figure(hp);
        plot(x, p, 'color', C{n});
        
        figure(hv);
        plot(x, v, 'color', C{n});
        n = n+1;
    end
    p = p - dt/(2*dx)*(Fu-Fd) + dt^2/(2*dx^2)*(qu.*(Fu-F) - qd.*(F-Fd));
end

figure(hp);
set(gca,'fontsize',24, 'fontweight', 'demi');
ylim([0, 1])
xlabel('x'); ylabel('\rho');
title('\eta = 0.1');
% legend('t=0', 't=6', 't=12', 't=18', 't=24', 't=30');

figure(hv);
set(gca,'fontsize',24, 'fontweight', 'demi');
ylim([0, 30])
xlabel('x'); ylabel('v');
title('\eta = 0.1');
% legend('t=0', 't=6', 't=12', 't=18', 't=24', 't=30');

saveas(hp, 'report/img/exc3_p_01.png');
saveas(hv, 'report/img/exc3_v_01.png');


eta = 0.9;
p = eta*pm*exp( -((x-L/4)/(L/8)).^2 )';
hp = figure; hold on;
hv = figure; hold on;
n=1;
for nLW=1:31
    pu = [p(2:end); p(1)];
    pd = [p(end); p(1:end-1)];
    %TODO: All these F's just become zero
    Fu = pu*vm.*(1-pu/pm);
    Fd = pd*vm.*(1-pd/pm);
    v = vm*(1-p/pm);
    F = p.*v;
    qu = vm*(1-(pu+p)/pm);
    qd = vm*(1-(p+pd)/pm);
    
    if mod(nLW-1, 6) == 0
        figure(hp);
        plot(x, p, 'color', C{n});
        
        figure(hv);
        plot(x, v, 'color', C{n});
        n = n+1;
    end
    p = p - dt/(2*dx)*(Fu-Fd) + dt^2/(2*dx^2)*(qu.*(Fu-F) - qd.*(F-Fd));
end

figure(hp);
set(gca,'fontsize',24, 'fontweight', 'demi');
ylim([0, 1])
xlabel('x'); ylabel('\rho');
title('\eta = 0.9');
% legend('t=0', 't=6', 't=12', 't=18', 't=24', 't=30');

figure(hv);
set(gca,'fontsize',24, 'fontweight', 'demi');
ylim([0, 30])
xlabel('x'); ylabel('v');
title('\eta = 0.9');
% legend('t=0', 't=6', 't=12', 't=18', 't=24', 't=30');

saveas(hp, 'report/img/exc3_p_09.png');
saveas(hv, 'report/img/exc3_v_09.png');

