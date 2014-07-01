close all;
C = {'k','b','r',[0.1, 0.7, 0.1],[0.7, 0, 0.5], 'm'};

vm = 25;
pm = 1;

NG = 40;
L = 400;
dx = L/NG;
dt = dx/vm; %TODO: choose better value for dt.
x = dx/2:dx:L-dx/2;

pinit = zeros(NG, 1);
pinit(NG/4-1) = pm/2;
pinit(NG*3/4+1) = pm/2;
pinit(NG/4:NG*3/4) = pm;

%% Lax method
%Initial values
p = pinit;
pu = zeros(NG, 1);
pd = zeros(NG, 1);

hp = figure; hold on;
hv = figure; hold on;
hF = figure; hold on;
nC = 1;
T=100;
for t=0:dt:T
    v  = vm.*(1-p/pm);
    F  = p.*v;
    pu = [p(2:end); p(1)];
    pd = [p(end); p(1:end-1)];
    Fu = pu*vm.*(1-pu/pm);
    Fd = pd*vm.*(1-pd/pm);
    
    p = 1/2*(pu+pd) - dt/(2*dx)*(Fu-Fd);
    
    %Plot
    if(t==0 || (t<=2 && t>2-dt) || (t<=5 && t>5-dt) || (t<=10 && t>10-dt/2) || (t<=T && t>T-dt/2))
        figure(hp)
        plot(x, p, 'color', C{nC});
        
        figure(hv)
        plot(x, v, 'color', C{nC})
        
        figure(hF)
        plot(x, F, 'color', C{nC})
        
        nC = nC+1;
    end
end

figure(hp)
set(gca,'fontsize',24, 'fontweight', 'demi');

ylim([0, 1.5]);
xlabel('x'); ylabel('\rho');
title('Lax method');

figure(hv)
set(gca,'fontsize',24, 'fontweight', 'demi');
ylim([0, 30]);
xlabel('x'); ylabel('v');
title('Lax method');

figure(hF)
set(gca,'fontsize',24, 'fontweight', 'demi');
ylim([0, 8]);
xlabel('x'); ylabel('F');
title('Lax method');

saveas(hp, 'report/img/exc2_Lax_p.png');
saveas(hv, 'report/img/exc2_Lax_v.png');
saveas(hF, 'report/img/exc2_Lax_F.png');


%% Lax-Wendroff method
%Initial values
p = pinit; %If set at zero, all terms in LW is zero so nothing happens, look at eq. 13
pnew = zeros(NG, 1);
pu = zeros(NG, 1);
pd = zeros(NG, 1);

hp = figure; hold on;
hv = figure; hold on;
hF = figure; hold on;
nC = 1;
T=100;
for t=0:dt:T
    pu = [p(2:end); p(1)];
    pd = [p(end); p(1:end-1)];
    %TODO: All these F's just become zero
    Fu = pu*vm.*(1-pu/pm);
    Fd = pd*vm.*(1-pd/pm);
    v  = vm*(1-p/pm);
    F = p.*v;
    qu = vm*(1-(pu+p)/pm);
    qd = vm*(1-(p+pd)/pm);
    
    if(t==0 || (t<=2 && t>2-dt) || (t<=5 && t>5-dt) || (t<=10 && t>10-dt/2) || (t<=T && t>T-dt/2))
        figure(hp)
        plot(x, p, 'color', C{nC});
        
        figure(hv)
        plot(x, v, 'color', C{nC})
        
        figure(hF)
        plot(x, F, 'color', C{nC})
        
        nC = nC+1;
    end
    p = p - dt/(2*dx)*(Fu-Fd) + dt^2/(2*dx^2)*(qu.*(Fu-F) - qd.*(F-Fd));
end

figure(hp)
set(gca,'fontsize',24, 'fontweight', 'demi');
ylim([0, 1.5]);
xlabel('x'); ylabel('\rho');
title('Lax-Wendroff method');

figure(hv)
set(gca,'fontsize',24, 'fontweight', 'demi');
ylim([0, 30]);
xlabel('x'); ylabel('v');
title('Lax-Wendroff method');

figure(hF)
set(gca,'fontsize',24, 'fontweight', 'demi');
ylim([0, 8]);
xlabel('x'); ylabel('F');
title('Lax-Wendroff method');

saveas(hp, 'report/img/exc2_LW_p.png');
saveas(hv, 'report/img/exc2_LW_v.png');
saveas(hF, 'report/img/exc2_LW_F.png');



