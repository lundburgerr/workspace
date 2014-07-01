@echo off

setlocal

set batch_dir=%~dp0

set REPLACESTRINGS=
if exist %batch_dir%\ReplaceStrings.exe set REPLACESTRINGS=%batch_dir%\ReplaceStrings.exe
if not defined REPLACESTRINGS (
echo Error: %batch_dir%\ReplaceStrings.exe not found!
echo.
goto end
)

set FUNCTIONLIST=USE_LEC internal_lec_init internal_lec calc_erle_fg_compensation calculate_filter_deviation filter_copy_bg2fg reset_bg_filter averages_init averages noise_est noise_gen_cnoi resdamp_init resdamp_detect_atalk resdamp_detect_btalk resdamp_tx_gains_org resdamp_delay_tx lec_subband_nlms lec_subband_fgfilt lec_check_performance lec_compare_filters lec_ls_delay_put lec_ls_delay_get
set VARIABLELIST=pcq31RX_fft pcq31LS_fft pcq31MIC_fft pcq31TX_fft pcq31MICE_bg_fft pcq31MICE_fg_fft pcq31AErr_bg_fft pcq31AErr_fg_fft pcq15NLMS_coefs_bg pcq15NLMS_coefs_fg pcq31NLMS_delay uiNLMS_delay_index pq31NLMS_energy pq15AErle_bg pq15AErle_fg pq15AErle_fg_compensation bAECUpdate bCopyFilter bResetFilter pq31Filter_div_bg pq31Filter_div_fg pcq31EYhat_bg_ave pcq31EYhat_fg_ave pcq31YYhat_bg_ave pcq31YYhat_fg_ave pcq31YhatYhat_fg_ave pq15AErr_damp_set pq15AErr_damp pq15AFeedback pq31GammaValue_bg pq15LastValue_bg puiMaxIndex_bg pq15MaxValue_bg pq31GammaValue_fg pq15LastValue_fg puiMaxIndex_fg pq31MIC_ave_slow pq31AErr_bg_ave_slow pq31AErr_fg_ave_slow pq31RX_ave pq31LS_ave pq31MIC_ave pq31TX_ave pq31MIChat_bg_ave pq31MIChat_fg_ave pq31AErr_bg_ave pq31AErr_fg_ave pq31LS_hold_ave pq31LS_tail_ave pq31LS_tail_abs pq31RX_noi_est pq31LS_noi_est pq31MIC_noi_est pq31LS_hold1 pq31LS_hold2 pq15Prot_delayLS pq15Prot_delayMIC pq31Prot_delayTX iProt_delayIndexLS iProt_delayIndexMIC iProt_delayIndexTX rxNoiEstSet lsNoiEstSet micNoiEstSet pcq31CNoi_fft pq15Prot_delayCNOI iProt_delayIndexCNOI pq31CNoi_ave q31DistEcho pq31echoEst pq31SpeechEst ppcq31TX_delay bBtalk_instant bBtalkStrong bAtalk bBtalk piBGtoFG_copy_counter piBG_reset_counter

rem LEC 2
%REPLACESTRINGS% %batch_dir%\..\include\lec2.h %batch_dir%\..\include\lec.h _2 %FUNCTIONLIST% %VARIABLELIST%
%REPLACESTRINGS% %batch_dir%\..\src\lec2.c %batch_dir%\..\src\lec.c _2 %FUNCTIONLIST% %VARIABLELIST%

%REPLACESTRINGS% %batch_dir%\..\include\lec2.h LEC_H LEC2_H
%REPLACESTRINGS% %batch_dir%\..\include\lec2.h lec.h lec2.h
%REPLACESTRINGS% %batch_dir%\..\src\lec2.c lec.h lec2.h


rem Create LEC 3
%REPLACESTRINGS% %batch_dir%\..\include\lec3.h %batch_dir%\..\include\lec.h _3 %FUNCTIONLIST% %VARIABLELIST%
%REPLACESTRINGS% %batch_dir%\..\src\lec3.c %batch_dir%\..\src\lec.c _3 %FUNCTIONLIST% %VARIABLELIST%

%REPLACESTRINGS% %batch_dir%\..\include\lec3.h LEC_H LEC3_H
%REPLACESTRINGS% %batch_dir%\..\include\lec3.h lec.h lec3.h
%REPLACESTRINGS% %batch_dir%\..\src\lec3.c lec.h lec3.h

:end

endlocal