
%% Initialize
close all;
global L;
global T;
global c;

L = 10;
T = 2;
c = 10;

dx = L/(2^7-1);
dt = dx/c*1;%(2/pi+0.01);

x = 0:dx:L;
t = 0:dt:T;
N = length(x);


%% A
f = @(x) exp(-(x-L/2).^2);
% 
% u = explicit(dx, dt, f);
% u = implicit(dx, dt, f);
% u = crank(dx, dt, f);
% u = ffttime(dx, dt, f); %TODO: Not sure about scaling and speed
% u = fftimm(dx, f, 0); %TODO: Not sure about scaling and speed
% for tt = t
%     figure(1);
%     uu = u(tt);
%     plot(x, uu(1:N)); ylim([-1 1]);
%     pause(0.02)
% end

%% B
f = @(x) sin(0.2*pi*x);
% 
% u = explicit(dx, dt, f);
% u = implicit(dx, dt, f);
% u = crank(dx, dt, f);
% u = ffttime(dx, dt/5, f);
% u = fftimm(dx, f, 'periodic');
% for tt = t
%     figure(1);
%     uu = u(tt);
%     plot(x, uu(1:N)); ylim([-1 1]);
%     pause(0.02)
% end

%% C
f = @(x) 1*(x>10/3 & x<20/3);
% 
% u = explicit(dx, dt, f);
% u = implicit(dx, dt, f);
% u = crank(dx, dt, f);
% u = ffttime(dx, dt/5, f); %TODO: Not sure about scaling and speed
% u = fftimm(dx, f, 0); %TODO: Not sure about scaling and speed
% for tt = t
%     figure(1);
%     uu = u(tt);
%     plot(x, uu(1:N)); ylim([-1 1]);
%     pause(0.02)
% end
% 
% h = figure;
% uu = u(0.3);
% plot(x, uu(1:N)); ylim([-1 1]);
% xlabel('x (m)'); ylabel('u(x)')
% legend('t=0.4s');
% saveas(h, 'report/img/discontFFTimm.png');





