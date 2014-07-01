close all

global a
global b
global c

a = 77.27;
b = 8.375*10^-6;
c = 1.161;

%% Solve ODE system
options = odeset('RelTol',1e-4,'AbsTol',[1e-7 1e-7 1e-7], 'Jacobian', @bzJac);
init = [1 2 3];
time = [0 360];

%Solve the equation
% sol45 = ode45(@bz, time, init, options);
tic
 sol23s = ode23s(@bz, time, init, options);
 toc
% sol15s = ode15s(@bz, time, init, options);

%% Plot
%ODE45
% hplot = figure;
t = sol45.x;
h = [t(2), t(2:end)-t(1:end-1)];
Y = deval(sol45, t);
sol45.stats

% xlabel('t'); ylabel('x_i');
%legend('x_1', 'x_2', 'x_3');
figure;
plot3(Y(1, :), Y(2, :), Y(3, :));
xlabel('x_1'); ylabel('x_2'); zlabel('x_3');
%saveas(hplot, 'exc5_ode45.png');

hplot = figure;
subplot(1,3,1);
plotyy(t,Y(1, :),t,h)
xlabel('t'); ylabel('x_1 and time step');
%legend('x_1', 'time step');

subplot(1,3,2);
plotyy(t,Y(2, :),t,h)
xlabel('t'); ylabel('x_2 and time step');
%legend('x_1', 'time step');

subplot(1,3,3);
plotyy(t,Y(3, :),t,h)
xlabel('t'); ylabel('x_3 and time step');
legend('x_1', 'time step');
%saveas(hplot, 'exc5_ode45YY.png');

%ODE23s
%hplot = figure;
t = sol23s.x;
h = [t(2), t(2:end)-t(1:end-1)];
Y = deval(sol23s, t);
sol23s.stats

% xlabel('t'); ylabel('x_i');
%legend('x_1', 'x_2', 'x_3');
figure;
plot3(Y(1, :), Y(2, :), Y(3, :));
xlabel('x_1'); ylabel('x_2'); zlabel('x_3');
%saveas(hplot, 'exc5_ode45.png');

hplot = figure;
subplot(1,3,1);
plotyy(t,Y(1, :),t,h)
xlabel('t'); ylabel('x_1 and time step');
%legend('x_1', 'time step');

subplot(1,3,2);
plotyy(t,Y(2, :),t,h)
xlabel('t'); ylabel('x_2 and time step');
%legend('x_1', 'time step');

subplot(1,3,3);
plotyy(t,Y(3, :),t,h)
xlabel('t'); ylabel('x_3 and time step');
legend('x_1', 'time step');
%saveas(hplot, 'exc5_ode45YY.png');

%ODE15s
%hplot = figure;
t = sol15s.x;
h = [t(2), t(2:end)-t(1:end-1)];
Y = deval(sol15s, t);
sol15s.stats

% subplot(1,3,1)
% plot(t, Y(1, :));xlabel('t'); ylabel('x_1');
% subplot(1,3,2)
% plot(t, Y(2, :));xlabel('t'); ylabel('x_2');
% subplot(1,3,3)
% plot(t, Y(3, :));xlabel('t'); ylabel('x_3');

% xlabel('t'); ylabel('x_i');
%legend('x_1', 'x_2', 'x_3');
figure;
plot3(Y(1, :), Y(2, :), Y(3, :));
xlabel('x_1'); ylabel('x_2'); zlabel('x_3');
%saveas(hplot, 'exc5_ode45.png');

hplot = figure;
subplot(1,3,1);
plotyy(t,Y(1, :),t,h)
xlabel('t'); ylabel('x_1 and time step');
%legend('x_1', 'time step');

subplot(1,3,2);
plotyy(t,Y(2, :),t,h)
xlabel('t'); ylabel('x_2 and time step');
%legend('x_1', 'time step');

subplot(1,3,3);
plotyy(t,Y(3, :),t,h)
xlabel('t'); ylabel('x_3 and time step');
legend('x_1', 'time step');
%saveas(hplot, 'exc5_ode45YY.png');


%% ODE45 with large tol
options = odeset('RelTol',1e0,'AbsTol',[1e0 1e0 1e0], 'Jacobian', @bzJac);
sol45high = ode45(@bz, time, init, options);

hplot = figure;
t = sol45high.x;
h = [t(2), t(2:end)-t(1:end-1)];
Y = deval(sol45high, t);
sol45high.stats

subplot(1,3,1)
plot(t, Y(1, :));xlabel('t'); ylabel('x_1');
subplot(1,3,2)
plot(t, Y(2, :));xlabel('t'); ylabel('x_2');
subplot(1,3,3)
plot(t, Y(3, :));xlabel('t'); ylabel('x_3');

% xlabel('t'); ylabel('x_i');
%legend('x_1', 'x_2', 'x_3');
figure;
plot3(Y(1, :), Y(2, :), Y(3, :));
xlabel('x_1'); ylabel('x_2'); zlabel('x_3');
%saveas(hplot, 'exc5_ode45.png');

hplot = figure;
subplot(3,1,1);
plotyy(t,Y(1, :),t,h)
xlabel('t'); ylabel('x_1 and time step');
%legend('x_1', 'time step');

subplot(3,1,2);
plotyy(t,Y(2, :),t,h)
xlabel('t'); ylabel('x_2 and time step');
%legend('x_1', 'time step');

subplot(3,1,2);
plotyy(t,Y(2, :),t,h)
xlabel('t'); ylabel('x_3 and time step');
legend('x_1', 'time step');
%saveas(hplot, 'exc5_ode45YY.png');


%% Save workspace
% save('exc5.mat');

