function [t, y, sol] = vdpintro ()

  %% usage:  vdpintro ()
  %%
  %% intro


global ee;  
global count ; 
ee = 1; 
count = 0; 
[t,y]=ode45(@vdp,[0 20],[2 0]);   
% simple time plot
figure(1);
plot(t,y(:,1));
title('trajectories');
% phase plot
figure(2);
plot(y(:, 1), y(:, 2));
title('simple phase plot');
% number of func
disp(count);
% compute the step size as for each step 
s = t(2:length(t)) - t(1:length(t)-1); 
% now, do this
sol = ode45(@vdp,[0 20],[2 0]);
% this is a solution structure that can be used to get continuous output
% that should be better for plotting

%% get 100 equally spaced samples on the time interval
tt = linspace(0,20, 100);
%% get the corresponding states
yy = deval(sol, tt);
%% plot the results again 
figure(3);
plot(yy(1, :), yy(2, :));
title('deval phase plot');
%% this should be smoother
end

function dy = vdp(t,y);
global count; 
global ee; 
count = count +1; 
dy = zeros(2,1); 
dy(1) = y(2); 
dy(2) = ee*(1 - y(1)^2) * y(2) - y(1);
end
