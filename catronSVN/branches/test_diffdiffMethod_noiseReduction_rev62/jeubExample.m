% Simple example on how to use PSDdiff_jeubNoise.m
% Author - Robin Lundberg
% E-mail - robin.lundberg@limesaudio.com


% hhat =  [-0.0113, -0.1845,0.2289,-0.0682,-0.0176,0.0367,-0.0278,0.0384,-0.0163,-0.0004,0.0074...
%     ,0.0113,-0.0075,0.0170,-0.0132,0.0155,0.0268,-0.0388,0.0257,-0.0120,-0.0361,0.0521,-0.0320...
%     ,0.0207,-0.0002,-0.0019,0.0292,-0.0186,0.0297,-0.0360,0.0031,-0.0056,-0.0001,0.0285...
%    ,-0.0213,0.0024,-0.0004,0.0055,-0.0051,-0.0077];

%Get signal
filtersize = 40;
if false
    [s, fs] = wavread('2013-05-31 - noiseAndTalk_noMovement.wav');
    x = s(:,2); %speech + noise
    y = s(:,1); %noise
    
    [yhat, hhat, e] = nlms(y(1:5000), x(1:5000), filtersize, 0.15);
    y = filter(hhat, 1, y);
    
    scalefactor = mean(abs(x(1:5000)))/mean(abs(y(1:5000)));
    y = scalefactor*y;

    x=x(60000:end);
    y=y(60000:end);
  
elseif true %TODO:Test this signal aswell
    [s, fs_file] = wavread('Mic2_plan3_rörelse.wav', [5.6, 6]*10^6);
    fs = 8000;
    s = resample(s, fs, fs_file);
    x = s(:,1); %speech + noise
    y = s(:,2); %noise
    
    [yhat, hhat, e] = nlms(y(1000:2000), x(1000:2000), filtersize, 0.15);
    y = filter(hhat, 1, y);
    
    scalefactor = sum(abs(x(1:5000)))/sum(abs(y(1:5000)));
    y = scalefactor*y;
end

[s_hat, s_hatpf, db_drop, diffdiff, diffdiff_ne, pldne_diff] = PSDdiff_jeubNoise(x, y, fs);
