%% generate include files 
close all
clear all

x=randn(1,10000);
x=x./max(abs(x))*32767;

write_to_inc(x, '../include', 'whiteNoise.inc')
plot(x);