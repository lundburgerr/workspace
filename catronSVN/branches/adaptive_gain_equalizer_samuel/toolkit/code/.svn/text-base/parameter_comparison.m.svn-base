% Compares parameters used in Westerlunds paper and in my simulations

% Author - Markus Borgh
% E-mail - markus.borgh@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2010-01-22
% Last modification - 2010-01-22

close all
clear all

R = 32; % Decimation ratio

% Westerlund, In paper
Apaper = 0.004;
Bpaper = 1e-6;

% Westerlund, In our simulations
Asim = 0.001; %0.0004
Bsim = 1.8e-5; %1.8e-5



% Change parameters since we use downsampling in our subband processing
% Nils: 8 kHz med 16 subband
% Vi: 16 kHz med 32 subband, decimering 32
% För varje subbandssample hos oss så borde det väl finnas 16st sample hos nils eftersom den inte är nedsamplad
alpha_paper = 1 - (1-Apaper)^(0.5*R);
beta_paper = (1+Bpaper)^(0.5*R) - 1;
alpha_sim = 1 - (1-Asim)^(0.5*R);
beta_sim = (1+Bsim)^(0.5*R) - 1;

% Our method
alpha_our = alpha_sim;%*4;
beta_our = 5e-3;


