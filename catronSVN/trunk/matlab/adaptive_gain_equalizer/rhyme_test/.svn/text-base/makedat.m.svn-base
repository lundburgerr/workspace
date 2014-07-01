% File for changing a .wav file into a .dat fixpoint [-32768, 32767] file.

%filePath = 'D:\LjudTester\20140120_catron_mrt\';
%name = '20140120_MRT_m2a_noise_zone1_channel_1_talmic_channel2_brusmic_extraStartbrus';
%name = '20140121_Robin_i_zone3brus_lufHeadset_och_brusmick';

filePath = 'D:\Work\Catron\rhyme-test\inspelat_eget\';
name = 'mos_drt_robin_120dbA_zon3';

[x_stereo, fs] = wavread([filePath,'\',name,'.wav']);

SIG_CHANNEL=1; %what channel is the signal in?
NOISE_REF_CHANNEL =2;

headsetMic = x_stereo(:,SIG_CHANNEL);
noiseMic = x_stereo(:,NOISE_REF_CHANNEL);

% As .dat file 16-bit
dat_file_name_headsetMic = 'headsetMic_mos_drt_robin_120dBA_zon3.dat';
write_to_dat(headsetMic*32768, pwd, dat_file_name_headsetMic);

% As .dat file 16-bit
dat_file_name_refMic = 'noiseMic_mos_drt_robin_120dBA_zon3.dat';
write_to_dat(noiseMic*32768, pwd, dat_file_name_refMic);