function [IRl, IRr] = directive_filter(el, az, fs)
    d = 0.6; %Distance between ears
    s = 300; %speed of sound

    az = (-az)+pi/2;
    
    while (az<-pi || az>pi)
        az = az - 2*pi*sign(az);
    end
    
    sourcePos = [cos(az), sin(az), 0];
    distL = norm(sourcePos + [d/2, 0, 0]);
    distR = norm(sourcePos - [d/2, 0, 0]);
    
    delayL = round(distL/s*fs);
    delayR = round(distR/s*fs);
    
    if delayL < delayR
        delayR = delayR-delayL;
        delayL = 0;
    else
        delayL = delayL-delayR;
        delayR = 0;
    end
    
%     gainL = (1/distL^2) * (az/pi*0.3+0.7);
%     gainR = (1/distR^2) * ((1-abs(az/pi))*0.3+0.7);
%     
%     if gainL > gainR
%         gainR = gainR/gainL
%         gainL = 1
%     else
%         gainL = gainL/gainR
%         gainR = 1
%     end
    
    gainfactor = abs(sin(az))*0.8+0.2;
    if (abs(az)<pi/2)
        gainL = gainfactor;
        gainR = 1;
    else
        gainL = 1;
        gainR = gainfactor;
    end

    L = 18;
    
    IRl_tmp = zeros(1,L);
    IRr_tmp = zeros(1,L);
    IRl_tmp(delayL+1) = gainL;
    IRr_tmp(delayR+1) = gainR;
    
    IRl = IRl_tmp;
    IRr = IRr_tmp;
end

function [IRl, IRr] = directive_filter_old(el, az, fs)
    %TODO: This only needs to be loaded once and made global, but how to???
    load hrtf
    % Room dimensions, MIC & LS position
    ROOM = [ 4 4 3 ]; % dx, dy, dz
    MIC_POS = [ 2 2 1.75 ];
    d = 0.2; %Distance between ears
    Rf = 0.25; % Wall reflection coefficient
    
    L = 128;
    
    % Calculate cartesian coordinates for RIR calculation
    LS_POS = [ sin(az)*cos(el), cos(az)*cos(el), sin(el) ] + MIC_POS;
    
    % Calculate RIR
    IRl_tmp = findIR_reverb(fs, Rf, L, ROOM, MIC_POS - [d/2 0 0], LS_POS, hrtf_l);
    IRr_tmp = findIR_reverb(fs, Rf, L, ROOM, MIC_POS + [d/2 0 0], LS_POS, hrtf_r);
    
    [s, fs_r]  = wavread('../adaptive_gain_equalizer/2013-03-27 - Normal_Speech.wav');
    s = resample(s, fs, fs_r);
    
    sl = filter(IRl_tmp,1,s);
    sr = filter(IRr_tmp,1,s);
    g = sqrt(var(s)/(0.5*var(sl)+0.5*var(sr)));
    IRl_tmp = g*IRl_tmp;
    IRr_tmp = g*IRr_tmp;
   
    if(max(abs(IRl_tmp))>1 ||  max(abs(IRr_tmp))>1)
        error('there exists a value in a filter that is >1 (or <-1). This will not work in fixed numbers');
    end
    
% %     use_speech_like_noise = true;
% %     if use_speech_like_noise
% %         %generate white noice for debugging
% %         %source http://www.mathworks.com/matlabcentral/answers/45239
% %         data = s/sqrt((sum(s.^2))/length(s));
% %         [a,g]= lpc(data,50);
% %         noise = randn(length(data),1);
% %         noise= filter(g,a,noise);
% %         %
% %     else
% %         noise = randn(length(data),1);
% %     end
% %     
% %     %set the poser of the noise to the power of s
% %     noise = noise*sqrt(var(s)/var(noise));
% %     
% %     nl = filter(IRl_tmp,1,noise);
% %     nr = filter(IRr_tmp,1,noise);
% %     testG = sqrt(var(noise)/(0.5*var(nl)+0.5*var(nr)))
% %     soundsc(noise,fs);
% %     
% % %     sl = filter(IRl_tmp,1,s);
% % %     sr = filter(IRr_tmp,1,s);
% % %     powerDiff = (var(s)/(0.5*var(sl)+0.5*var(sr)))
% % %     soundsc(s,fs);
% % %     soundsc([sl,sr],fs);

    IRl = IRl_tmp;
    IRr = IRr_tmp;
end

