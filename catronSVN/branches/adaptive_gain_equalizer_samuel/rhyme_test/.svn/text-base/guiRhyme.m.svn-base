function varargout = guiRhyme(varargin)
% GUIRHYME MATLAB code for guiRhyme.fig
%      GUIRHYME, by itself, creates a new GUIRHYME or raises the existing
%      singleton*.
%
%      H = GUIRHYME returns the handle to a new GUIRHYME or the handle to
%      the existing singleton*.
%
%      GUIRHYME('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in GUIRHYME.M with the given input arguments.
%
%      GUIRHYME('Property','Value',...) creates a new GUIRHYME or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before guiRhyme_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to guiRhyme_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help guiRhyme

% Last Modified by GUIDE v2.5 19-Jun-2013 18:44:00

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @guiRhyme_OpeningFcn, ...
                   'gui_OutputFcn',  @guiRhyme_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT

% --- Executes just before guiRhyme is made visible.
function guiRhyme_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to guiRhyme (see VARARGIN)

% Choose default command line output for guiRhyme
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes guiRhyme wait for user response (see UIRESUME)
% uiwait(handles.figure1);
global Words;
global Sounds;
global Pos;
global Map;
global Wordgroups;
global Groupsize;
global Fs;

set(handles.right,'String',0);
set(handles.wrong,'String',0);
set(handles.percentage,'String',strcat(num2str(100),'%'));

sentence = '';
for k = 1:Groupsize
    sentence = strcat(sentence, ', ', Words{Pos(1), k});
end
set(handles.guessWord,'String','');
set(handles.words,'String',sentence);
set(handles.wordgroup,'String',{'hej','tjej'});
sound(Sounds{Pos(1), Pos(2)}, Fs);


% --- Outputs from this function are returned to the command line.
function varargout = guiRhyme_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in play.
function play_Callback(hObject, eventdata, handles)
% hObject    handle to play (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global Words;
global Sounds;
global Pos;
global Map;
global Wordgroups;
global Groupsize;
global Fs;

sound(Sounds{Pos(1), Pos(2)}, Fs);


function guessWord_Callback(hObject, eventdata, handles)
% hObject    handle to guessWord (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of guessWord as text
%        str2double(get(hObject,'String')) returns contents of guessWord as a double
% key = get(gcf,'CurrentCharacter');
% submitWord_Callback(hObject, eventdata, handles);



% --- Executes during object creation, after setting all properties.
function guessWord_CreateFcn(hObject, eventdata, handles)
% hObject    handle to guessWord (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in submitWord.
function submitWord_Callback(hObject, eventdata, handles)
% hObject    handle to submitWord (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global Words;
global Sounds;
global Pos;
global Map;
global Wordgroups;
global Groupsize;
global Fs;

%Check if guess is right and update score
if hObject == handles.guessWord
    guess = get(hObject, 'String');
else
    guess = get(handles.guessWord, 'String');
end
set(handles.guessWord, 'String', '');
right = str2num(get(handles.right,'String'));
wrong = str2num(get(handles.wrong,'String'));
if strcmp(guess, Words{Pos(1), Pos(2)})
    right = right+1;
    set(handles.right,'String',right);
    set(handles.wrong,'String',wrong);
    set(handles.percentage,'String',strcat(num2str(100*right/(wrong+right)),'%'));
else
    wrong = wrong+1;
    set(handles.right,'String',right);
    set(handles.wrong,'String',wrong);
    set(handles.percentage,'String',strcat(num2str(100*right/(wrong+right)),'%'));
end
    
%Setting new position
[row, col] = find(Map);
if length(row) == 0
    Map = ones(size(Sounds));
    [row, col] = find(Map);
end
newInd = ceil(rand()*length(row));
Pos = [row(newInd), col(newInd)];
Map(row(newInd), col(newInd)) = 0;

%Fill in new words
sentence = '';
for k = 1:Groupsize
    sentence = strcat(sentence, ', ', Words{Pos(1), k});
end
set(handles.words,'String',sentence);

%Play new sound
sound(Sounds{Pos(1), Pos(2)}, Fs);

% --- Executes on selection change in wordgroup.
function wordgroup_Callback(hObject, eventdata, handles)
% hObject    handle to wordgroup (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns wordgroup contents as cell array
%        contents{get(hObject,'Value')} returns selected item from wordgroup


% --- Executes during object creation, after setting all properties.
function wordgroup_CreateFcn(hObject, eventdata, handles)
% hObject    handle to wordgroup (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on key press with focus on guessWord and none of its controls.
function guessWord_KeyPressFcn(hObject, eventdata, handles)
% hObject    handle to guessWord (see GCBO)
% eventdata  structure with the following fields (see UICONTROL)
%	Key: name of the key that was pressed, in lower case
%	Character: character interpretation of the key(s) that was pressed
%	Modifier: name(s) of the modifier key(s) (i.e., control, shift) pressed
% handles    structure with handles and user data (see GUIDATA)
key = get(gcf,'CurrentKey');
pause(0.1);
if strcmp(key, 'return')
	submitWord_Callback(hObject, eventdata, handles);
end
