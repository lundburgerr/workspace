close all;

%% Exercise 1
!./exercise1 45 10 > log.txt 2>errlog.txt
h = figure;
res = load('log.txt');
t = res(:,1); x1 = res(:,2); x2 = res(:,3);
plot(t, x1, t, x2);
xlabel('time'); ylabel('x_i');
legend('x1', 'x2');
fid = fopen('errlog.txt');
title(sprintf('Van der Pol. rkf45, mu = 10, %s, %s', fgetl(fid), fgetl(fid)));
fclose(fid);
saveas(h, '/home/lundburgerr/workspace/gsl_ode/gsl_ode/report/img/exc1_45_10.png')
!rm log.txt
!rm errlog.txt

!./exercise1 45 1 > log.txt 2>errlog.txt
h = figure;
res = load('log.txt');
t = res(:,1); x1 = res(:,2); x2 = res(:,3);
plot(t, x1, t, x2);
xlabel('time'); ylabel('x_i');
legend('x1', 'x2');
fid = fopen('errlog.txt');
title(sprintf('Van der Pol. rkf45, mu = 1, %s, %s', fgetl(fid), fgetl(fid)));
fclose(fid);
saveas(h, '/home/lundburgerr/workspace/gsl_ode/gsl_ode/report/img/exc1_45_1.png')
!rm log.txt
!rm errlog.txt

!./exercise1 2 10 > log.txt 2>errlog.txt
h = figure;
res = load('log.txt');
t = res(:,1); x1 = res(:,2); x2 = res(:,3);
plot(t, x1, t, x2);
xlabel('time'); ylabel('x_i');
legend('x1', 'x2');
fid = fopen('errlog.txt');
title(sprintf('Van der Pol. rk2simp, mu = 10, %s, %s', fgetl(fid), fgetl(fid)));
fclose(fid);
saveas(h, '/home/lundburgerr/workspace/gsl_ode/gsl_ode/report/img/exc1_2_10.png')
!rm log.txt
!rm errlog.txt

!./exercise1 2 1 > log.txt 2>errlog.txt
h = figure;
res = load('log.txt');
t = res(:,1); x1 = res(:,2); x2 = res(:,3);
plot(t, x1, t, x2);
xlabel('time'); ylabel('x_i');
legend('x1', 'x2');
fid = fopen('errlog.txt');
title(sprintf('Van der Pol. rk2simp, mu = 1, %s, %s', fgetl(fid), fgetl(fid)));
fclose(fid);
saveas(h, '/home/lundburgerr/workspace/gsl_ode/gsl_ode/report/img/exc1_2_1.png')
!rm log.txt
!rm errlog.txt

%% Exercise 2
%TODO: Why does this result suck????
!./exercise2 2 60 > log.txt 2>errlog.txt
res = load('log.txt');
h = figure;
t = res(:,1);
y1 = res(:,2);
y2 = res(:,3);
y3 = res(:,4);
plot(t, y1, t, y2, t, y3);
xlabel('time'); ylabel('y_i');
legend('y1', 'y2', 'y3');
fid = fopen('errlog.txt');
title(sprintf('Chemical Kinetics. rk2imp, t = 60, %s, %s', fgetl(fid), fgetl(fid)));
fclose(fid);
saveas(h, '/home/lundburgerr/workspace/gsl_ode/gsl_ode/report/img/exc2_2_60.png')
!rm log.txt
!rm errlog.txt

!./exercise2 2 360 > log.txt 2>errlog.txt
res = load('log.txt');
h = figure;
t = res(:,1);
y1 = res(:,2);
y2 = res(:,3);
y3 = res(:,4);
plot(t, y1, t, y2, t, y3);
xlabel('time'); ylabel('y_i');
legend('y1', 'y2', 'y3');
fid = fopen('errlog.txt');
title(sprintf('Chemical Kinetics. rk2imp, t = 360, %s, %s', fgetl(fid), fgetl(fid)));
fclose(fid);
saveas(h, '/home/lundburgerr/workspace/gsl_ode/gsl_ode/report/img/exc2_2_360.png')
!rm log.txt
!rm errlog.txt

!./exercise2 4 60 > log.txt 2>errlog.txt
res = load('log.txt');
h = figure;
t = res(:,1);
y1 = res(:,2);
y2 = res(:,3);
y3 = res(:,4);
plot(t, y1, t, y2, t, y3);
xlabel('time'); ylabel('y_i');
legend('y1', 'y2', 'y3');
fid = fopen('errlog.txt');
title(sprintf('Chemical Kinetics. rk4imp, t = 60, %s, %s', fgetl(fid), fgetl(fid)));
fclose(fid);
saveas(h, '/home/lundburgerr/workspace/gsl_ode/gsl_ode/report/img/exc2_4_60.png')
!rm log.txt
!rm errlog.txt

!./exercise2 4 360 > log.txt 2>errlog.txt
res = load('log.txt');
h = figure;
t = res(:,1);
y1 = res(:,2);
y2 = res(:,3);
y3 = res(:,4);
plot(t, y1, t, y2, t, y3);
xlabel('time'); ylabel('y_i');
legend('y1', 'y2', 'y3');
fid = fopen('errlog.txt');
title(sprintf('Chemical Kinetics. rk4imp, t = 360, %s, %s', fgetl(fid), fgetl(fid)));
fclose(fid);
saveas(h, '/home/lundburgerr/workspace/gsl_ode/gsl_ode/report/img/exc2_4_360.png')
!rm log.txt
!rm errlog.txt

%% Exercise 3
!./exercise3 3 10 > log.txt 2>errlog.txt
res = load('log.txt');
h = figure;
t = res(:,1);
y1 = res(:,2);
y2 = res(:,3);
y3 = res(:,4);
plot(t, y1, t, y2, t, y3);
xlabel('time'); ylabel('organism');
legend('prey', 'predator', 'grass');
fid = fopen('errlog.txt');
title(sprintf('Predator prey. gamma = 3, %s, %s', fgetl(fid), fgetl(fid)));
fclose(fid);
saveas(h, '/home/lundburgerr/workspace/gsl_ode/gsl_ode/report/img/exc3_3.png')
!rm log.txt
!rm errlog.txt

!./exercise3 7 10 > log.txt 2>errlog.txt
res = load('log.txt');
h = figure;
t = res(:,1);
y1 = res(:,2); %prey
y2 = res(:,3); %predator
y3 = res(:,4); %grass
plot(t, y1, t, y2, t, y3);
xlabel('time'); ylabel('organism');
legend('prey', 'predator', 'grass');
fid = fopen('errlog.txt');
title(sprintf('Predator prey. gamma = 7, %s, %s', fgetl(fid), fgetl(fid)));
fclose(fid);
saveas(h, '/home/lundburgerr/workspace/gsl_ode/gsl_ode/report/img/exc3_7.png')
!rm log.txt
!rm errlog.txt

%% Exercise 4
!./exercise4
h = figure;
hare = load('hare.txt');
lynx = load('lynx.txt');
t = load('time.txt');
plot(t, hare, t, lynx);
xlabel('time'); ylabel('animal');
legend('prey', 'predator');
saveas(h, '/home/lundburgerr/workspace/gsl_ode/gsl_ode/report/img/exc4.png')
