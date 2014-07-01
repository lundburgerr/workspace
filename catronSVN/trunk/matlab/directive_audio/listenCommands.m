function listenCommands
%angles given in degrees [az1,el1; az2,el2; ... ]
%wavfile is the file from which to play the commands, maybe should just be
%fixed.

angles = [-100,0; -20,40; 0,0; 20,-40; 100,0];

[s, fs]  = wavread('../adaptive_gain_equalizer/2013-03-27 - Normal_Speech.wav');
s = resample(s, 8000, fs);
fs = 8000;
s = s(1:floor(length(s)/2));
N = floor(length(s)/5);
commands = [s(1:N), s(1+N:2*N), s(1+2*N:3*N), s(1+3*N:4*N), s(1+4*N:5*N)];



% 
% for n=1:5
%     % Azimuth and elevations (degrees => radians)
%     az_r = (angles(n,1)/360)*2*pi;
%     el_r = (angles(n,2)/360)*2*pi;
%     
%     % Calculate cartesian coordinates for RIR calculation
%     LS_POS = [ sin(az_r)*cos(el_r), cos(az_r)*cos(el_r), sin(el_r) ] + MIC_POS;
%     
%     % Calculate RIR
%     IRl_tmp = findIR_reverb(44100, Rf, L*3, ROOM, MIC_POS - [d/2 0 0], LS_POS, hrtf_l);
%     IRr_tmp = findIR_reverb(44100, Rf, L*3, ROOM, MIC_POS + [d/2 0 0], LS_POS, hrtf_r);
%     
%     % Resample to audio sampling frequency
%     IRl = [IRl, resample(IRl_tmp, fs, 44100)];
%     IRr = [IRr, resample(IRr_tmp, fs, 44100)];
% end
% 
% 
% 
% 


%Gui varies az and el angles aswell as mute and delay of signal
% for t=1:5
%     %TODO: Can this be calculated in another thread as the sound will be in
%     command_sound_r = zeros(N+max(delay)*fs,1);
%     command_sound_l = zeros(N+max(delay)*fs,1);
%     for n=1:5
%         silence = zeros(round(delay(n)*fs),1);
%         append = length(command_sound_r)-length(silence)-L_command;
%         command_sound_r = command_sound_r + [silence; filter(IRr(:,n), 1, commands(:,n)); zeros(append,1)];
%         command_sound_l = command_sound_l + [silence; filter(IRl(:,n), 1, commands(:,n)); zeros(append,1)];
%     end
%     
%     sound([command_sound_l, command_sound_r], fs);  
% end

IRl = [];
IRr = [];
%L_command = length(filter(IRr(:,1), 1, commands(:,1)));
L_command = length(commands(:,1));
global elevation; %Array of size 5 with all elevation angles
global azimuthal; %Array of size 5 with all azimuthal angles
global mute; %Array of size 5 with bools indicating muted or not, true indicates muted
global delay; %Array of size 5 with delays for all commands in seconds
global changeAngles; %Bool that indicates if something was changed
global changeMute;
global exit;

elevation = zeros(5,1);
azimuthal = zeros(5,1);
mute = zeros(5,1);
delay = zeros(5,1);
changeAngles = ones(5,1); %All ones in order to calculate all filters once
changeMute = zeros(5,1);
exit = false;

gui();
command_sound_r = zeros(N+5*fs,5); %maximum delay is 5 seconds
command_sound_l = zeros(N+5*fs,5); %maximum delay is 5 seconds
left_sound = zeros(N+5*fs,1);
right_sound = zeros(N+5*fs,1);
while ~exit,
%TODO: Can this be calculated in another thread as the sound will be in
if ~isempty(find(changeMute, 1)) || ~isempty(find(changeAngles, 1))
    %check el and az is changed in that case calculate new filter
    %Calculate all new sounds
    tic
    for n=find(changeAngles)'
        changeAngles(n) = false;
        [IRl(:,n), IRr(:,n)] = directive_filter(elevation(n), azimuthal(n), fs);
        silence = zeros(round(delay(n)*fs),1);
        append = length(command_sound_r)-length(silence)-L_command;
        command_sound_r(:,n) = [silence; filter(IRr(:,n), 1, commands(:,n)); zeros(append,1)];
        command_sound_l(:,n) = [silence; filter(IRl(:,n), 1, commands(:,n)); zeros(append,1)];
    end
    
    for n=find(changeMute)'
        changeMute(n) = false;
        if mute(n)
            command_sound_r(:,n) = zeros(N+5*fs,1); %maximum delay is 5 seconds
            command_sound_l(:,n) = zeros(N+5*fs,1); %maximum delay is 5 seconds
        else
            silence = zeros(round(delay(n)*fs),1);
            append = length(command_sound_r)-length(silence)-L_command;
            command_sound_r(:,n) = [silence; filter(IRr(:,n), 1, commands(:,n)); zeros(append,1)];
            command_sound_l(:,n) = [silence; filter(IRl(:,n), 1, commands(:,n)); zeros(append,1)];
        end
            
    end
  
    %Add together new sound
    %TODO: It's enough to just subtract old sound and adding new sound
    %instead of summing it all over again
    left_sound = zeros(N+5*fs,1);
    right_sound = zeros(N+5*fs,1);
    for n=1:5
        left_sound = left_sound + command_sound_l(:,n);
        right_sound = right_sound + command_sound_r(:,n);
    end
    %TODO: Below is a bit of fulfix to remove most of the trailing zeros
    stopAt = find(left_sound, 1, 'last')+round(fs/10);
    if stopAt > (N+5*fs)
        stopAt = N+5*fs;
    end
    left_sound = left_sound(1:stopAt);
    right_sound = right_sound(1:stopAt);
    toc
end

sound([left_sound, right_sound], fs);
pause(0.05);

end
