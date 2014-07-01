%RESULT_CMOS - Display results from CMOS testing 8 audio files. A positive
%value is good for the proposed algorithm

close all
clear all

% Number of test persons
Ntp = 7;
% Number of files
Nf = 8;

res = zeros(Ntp, Nf);
order = zeros(Ntp, Nf);

res(1,:) = [-2, 2, 1, 2, 2, -2, 0, 1];
res(2,:) = [2, -1, -1, 1, 1, -1, -1, 2];
res(3,:) = [1, 1, -1, 0, 0, -2, 0, 2];
res(4,:) = [2, -1, 1, 0, 1, -1, 2, -2];
res(5,:) = [3, -1, 1, 2, -1, 3, -2, -2];
res(6,:) = [-1, -1, -1, -2, -1, 1, 1, 1];
res(7,:) = [-2, -2, 0, 2, -1, 2, -1, 0];
% res(1,:) = [];
% res(1,:) = [];

order(1,:) = [1, -1, -1, -1, -1, 1, 1, -1];
order(2,:) = [-1, 1, -1, -1, -1, 1, 1, -1];
order(3,:) = [-1, -1, -1, -1, -1, 1, -1, -1];
order(4,:) = [-1, 1, 1, -1, 1, 1, 1, -1];
order(5,:) = [1, -1, -1, 1, 1, 1, -1, -1];
order(6,:) = [-1, -1, 1, 1, 1, -1, -1, -1];
order(7,:) = [-1, -1, -1, 1, -1, 1, -1, 1];
% order(2,:) = [];
% order(2,:) = [];


res_order = res.*order;

res_mean = mean(res_order);
res_std = std(res_order);

% plot(res_mean,'*')
errorbar(1:Nf, res_mean, res_std./Ntp)
axis([0, Nf+1, -3, 3])
grid on
title('Comparison Mean Opinion Score, errorbars show std/(# of test persons)')
ylabel('CMOS')
xlabel('File')