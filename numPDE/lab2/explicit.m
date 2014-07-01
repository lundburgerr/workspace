function u = explicit(dx, dt, f)

global L;
global T;
global c;

x = 0:dx:L;
t = 0:dt:T;

N = length(x)-2;
u = f(x(2:end-1))';
uold = u;

% h = figure; hold on;
% color = {'b', 'r'};
% nn = 1;
for ii=1:length(t)
    tmp = u;
    uu = [u(2:end); 0];
    ud = [0; u(1:end-1)];
    u = 2*u - uold + (c*dt/dx)^2 *(uu-2*u+ud);
    
    uold = tmp;
        
    figure(1); hold off;
    plot(x, [0;u;0]); ylim([-1 1]);
    pause(0.002);
%     if( t(ii) == 0 || (t(ii) < 0.4+dt/2 && t(ii) > 0.4-dt/2))
%     if(t(ii) < 0.3+dt/2 && t(ii) > 0.3-dt/2)
%         plot(x, [0;u;0], color{nn}); ylim([-1 1]);
%         nn = nn+1;
%     end
end
% xlabel('x (m)'); ylabel('u(x)')
% legend('t=0.4 s');
% saveas(h, 'report/img/discontExp.png');
u = [0;u;0];