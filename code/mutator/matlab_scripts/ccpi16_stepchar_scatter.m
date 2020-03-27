clear all
close all

chfl='/home/balaji/clang-llvm/mutationdocker/code/mutants_ccpi16a';

% Get a list of all files and folders in this folder.
dir_files = dir(chfl);
% Get a logical vector that tells which is a directory.
dirFlags = [dir_files.isdir];
% Extract only those that are directories.
folder_names = dir_files(dirFlags);

for i = 3:length(folder_names)
    xlabel_dataout{i} = getfield(folder_names(i),'name');
end

%load base values to compare with
load('/home/balaji/clang-llvm/mutationdocker/code/mutants_ccpi16a/ccpi_original.mat')
base_value= tout(2,:);
base_sti = stepinfo(tout(2,:),tout(1,:),10);

overall_index=0;
hi_th=10;
lo_th=0;
even_space_multiples= 1; %11
fig_index=1;
fig_row=3;

for th_loop=linspace(lo_th,hi_th,even_space_multiples)

folder_index=1;
mutant_index=1;

running_mutants_per_order = [];
blowing_mutants_per_order = [];

mutant_templates = [];
step_charac_RiseTime = [];
step_charac_SettlingTime = [];
step_charac_SettlingMin = [];
step_charac_SettlingMax = [];
step_charac_Overshoot = [];
step_charac_Undershoot = [];
step_charac_Peak = [];
step_charac_PeakTime = [];

%four characterization models for airspeed
airspeed_error_m1 = [];
airspeed_error_m2 = [];
airspeed_error_m3 = [];
airspeed_error_m4 = [];

%four characterization models for airspeed for non blowing models only
airspeed_error_non_blow_m1 = [];
airspeed_error_non_blow_m2 = [];
airspeed_error_non_blow_m3 = [];
airspeed_error_non_blow_m4 = [];

x_index =1;
all_mutants = [];
no_of_templates=length(folder_names)-2;
live_mutants = zeros(1,no_of_templates);
dead_mutants = zeros(1,no_of_templates);
dead_mutants_param = zeros(8,no_of_templates);
live_mutants_param = zeros(8,no_of_templates);
xin = 0:(no_of_templates-1);
total_mutants = 0;

threshold_value=th_loop; 
threshold_percenage=threshold_value/100; %10 percentage
threshold_RiseTime = threshold_value; % base_sti.RiseTime  value is zero 
threshold_SettlingTime = threshold_value; % base_sti.SettlingTime value is Nan
threshold_SettlingMin = threshold_percenage * base_sti.SettlingMin;
threshold_SettlingMax = threshold_percenage * base_sti.SettlingMax;
threshold_Overshoot = threshold_percenage * base_sti.Overshoot;
threshold_Undershoot = threshold_value; % base_sti.Undershoot value is Nan
threshold_Peak = threshold_percenage * base_sti.Peak;
threshold_PeakTime = threshold_percenage * base_sti.PeakTime;


retain_total_mutants=0;
for folder_list = 3:numel(folder_names)
    mut_folder=strcat('/home/balaji/clang-llvm/mutationdocker/code/mutants_ccpi16a/',folder_names(folder_list).name);
    mut_folder
    
    cd(mut_folder)
    listoffiles = dir('*.c');
    C_nooffiles = numel(listoffiles);
    listoffiles = dir('*.txt');
    TXT_nooffiles = numel(listoffiles);
    listoffiles = dir('*.*');
    ALL_nooffiles = numel(listoffiles);
    log_txt_files=dir('log2_*.txt');
    
    
    nooffiles=C_nooffiles;
%     nooffiles
    
    all_mutants(x_index)=nooffiles;
    total_mutants = total_mutants + nooffiles;

    %make sure we are at the right place before the simulation starts
    home = '/home/balaji/clang-llvm/mutationdocker/code/mutants_ccpi16a';
    cd(home)

    format long;
    %load base values to compare with
    load('/home/balaji/clang-llvm/mutationdocker/code/mutants_ccpi16a/ccpi_original.mat')
    base_value = tout(2,:);

    %Error order
    error_order = 0;
    blow_mutant_count = 0;
    running_mutant_count = 0;

    %four characterization models for airspeed
    airspeed_error_m1_order = 0;
    airspeed_error_m2_order = 0;
    airspeed_error_m3_order = 0;
    airspeed_error_m4_order = 0;

    %four characterization models for airspeed for non blowing models only
    airspeed_error_non_blow_m1_order = 0;
    airspeed_error_non_blow_m2_order = 0;
    airspeed_error_non_blow_m3_order = 0;
    airspeed_error_non_blow_m4_order = 0;

%     step_charac_RiseTime(folder_index,1)= -1000;
%     step_charac_SettlingTime(folder_index,1)= -1000;
%     step_charac_SettlingMin(folder_index,1)= -1000;
%     step_charac_SettlingMax(folder_index,1)= -1000;
%     step_charac_Overshoot(folder_index,1)= -1000;
%     step_charac_Undershoot(folder_index,1)= -1000;
%     step_charac_Peak(folder_index,1)= -1000;
%     step_charac_PeakTime(folder_index,1)= -1000;
    
    step_charac_RiseTime(folder_index,1)= 0;
    step_charac_SettlingTime(folder_index,1)= 0;
    step_charac_SettlingMin(folder_index,1)= 0;
    step_charac_SettlingMax(folder_index,1)= 0;
    step_charac_Overshoot(folder_index,1)= 0;
    step_charac_Undershoot(folder_index,1)= 0;
    step_charac_Peak(folder_index,1)= 0;
    step_charac_PeakTime(folder_index,1)= 0;
    
    %for mutant 
    for i = 1:numel(log_txt_files)
       
        %Generate right file paths
        file_path = mut_folder;
        fid = fopen([file_path '/' log_txt_files(i).name],'r');
        
%         fid = fopen([file_path '/log2_' int2str(i) '.txt'],'r');
        if fid == -1
            error('Author:Function:OpenFile', 'Cannot open file: %s', [file_path '/' i]);
%             error('Author:Function:OpenFile', 'Cannot open file: %s', [file_path '/log2_' int2str(i) '.txt']);
        end
        
        log= textscan(fid,'%s');
        log = log{1};
        %if the mutant failed execution/or was not compiled successfully
        if (strcmp(log{1}, 'NO')) 
%             step_charac_RiseTime(folder_index,overall_index)= -1000;
%             step_charac_SettlingTime(folder_index,overall_index)= -1000;
%             step_charac_SettlingMin(folder_index,overall_index)= -1000;
%             step_charac_SettlingMax(folder_index,overall_index)= -1000;
%             step_charac_Overshoot(folder_index,overall_index)= -1000;
%             step_charac_Undershoot(folder_index,overall_index)= -1000;
%             step_charac_Peak(folder_index,overall_index)= -1000;
%             step_charac_PeakTime(folder_index,overall_index)= -1000;
            fclose(fid);
            continue 
        end
        fclose(fid);
        
        [log_filepath,log_name,log_ext]=fileparts(log_txt_files(i).name);
        log_fileid = strsplit(log_name,'_');
        
        file = [file_path '/ccpi16a_' log_fileid{2} '.mat'];
        load(file)
        sti = stepinfo(tout(2,:),tout(1,:),10);

        %increase mutant count
        running_mutant_count = running_mutant_count + 1;
        is_blow_up = false;
        
        overall_index = mutant_index + retain_total_mutants;
%         overall_index = mutant_index + (folder_index-1);
        %mutant_templates(overall_index)=str2num(folder_names(folder_list).name);
        
        step_charac_RiseTime(folder_index,overall_index)= sti.RiseTime;
        step_charac_SettlingTime(folder_index,overall_index)= sti.SettlingTime;
        step_charac_SettlingMin(folder_index,overall_index)= sti.SettlingMin;
        step_charac_SettlingMax(folder_index,overall_index)= sti.SettlingMax;
        step_charac_Overshoot(folder_index,overall_index)= sti.Overshoot;
        step_charac_Undershoot(folder_index,overall_index)= sti.Undershoot;
        step_charac_Peak(folder_index,overall_index)= sti.Peak;
        step_charac_PeakTime(folder_index,overall_index)= sti.PeakTime;

        %Determine live or dead mutants
%         disp sti.RiseTime
%         sti.RiseTime
%         disp base_sti.RiseTime
%         base_sti.RiseTime
%         disp threshold_RiseTime
%         threshold_RiseTime
        
        
        
        if (~is_blow_up) && (abs(sti.RiseTime-base_sti.RiseTime) > threshold_RiseTime)
            is_blow_up = true;
            dead_mutants_param(1,x_index)=dead_mutants_param(1,x_index)+1;
        else
            live_mutants_param(1,x_index)=live_mutants_param(1,x_index)+1;
        end
%         is_blow_up
        if (~is_blow_up) && (abs(sti.SettlingTime-base_sti.SettlingTime) > threshold_SettlingTime)
            is_blow_up = true;
            dead_mutants_param(2,x_index)=dead_mutants_param(2,x_index)+1;
        else
            live_mutants_param(2,x_index)=live_mutants_param(2,x_index)+1;
        end
%         is_blow_up
        if (~is_blow_up) && (abs(sti.SettlingMin-base_sti.SettlingMin) > threshold_SettlingMin)
            is_blow_up = true;
            dead_mutants_param(3,x_index)=dead_mutants_param(3,x_index)+1;
        else
            live_mutants_param(3,x_index)=live_mutants_param(3,x_index)+1;
        end
%         is_blow_up
        if (~is_blow_up) && (abs(sti.SettlingMax-base_sti.SettlingMax) > threshold_SettlingMax)
            is_blow_up = true;
            dead_mutants_param(4,x_index)=dead_mutants_param(4,x_index)+1;
        else
            live_mutants_param(4,x_index)=live_mutants_param(4,x_index)+1;
        end
%         is_blow_up
        if (~is_blow_up) && ( sti.Overshoot > (base_sti.Overshoot + threshold_Overshoot) )
            is_blow_up = true;
            dead_mutants_param(5,x_index)=dead_mutants_param(5,x_index)+1;
        else
            live_mutants_param(5,x_index)=live_mutants_param(5,x_index)+1;
        end
        
%         is_blow_up
        if (~is_blow_up) && ( sti.Undershoot > (base_sti.Undershoot + threshold_Undershoot)  )
            is_blow_up = true;
            dead_mutants_param(6,x_index)=dead_mutants_param(6,x_index)+1;
        else
            live_mutants_param(6,x_index)=live_mutants_param(6,x_index)+1;
        end
%         is_blow_up
        if (~is_blow_up) && (abs(sti.Peak-base_sti.Peak) > threshold_Peak)
            is_blow_up = true;
            dead_mutants_param(7,x_index)=dead_mutants_param(7,x_index)+1;
        else
            live_mutants_param(7,x_index)=live_mutants_param(7,x_index)+1;
        end
%         is_blow_up
        if (~is_blow_up) && (abs(sti.PeakTime-base_sti.PeakTime) > threshold_PeakTime)
            is_blow_up = true;        
            dead_mutants_param(8,x_index)=dead_mutants_param(8,x_index)+1;
        else
            live_mutants_param(8,x_index)=live_mutants_param(8,x_index)+1;
        end
        
%         is_blow_up
        if(is_blow_up)
            dead_mutants(x_index) = dead_mutants(x_index) + 1;
        else
            live_mutants(x_index) = live_mutants(x_index) + 1;
        end
        
        mutant_index = mutant_index+1;
    end
    
    retain_total_mutants=overall_index;
%     folder_index = mutant_index + 1;
    folder_index=folder_index+1;
    folder_index
    mutant_index = 1;
    x_index = x_index+1;
%     step_charac_RiseTime

%     x=1:1:nooffiles;
    %plot(x,step_charac_RiseTime,x,step_charac_SettlingTime,x,step_charac_SettlingMin,x,step_charac_SettlingMax,x,step_charac_Overshoot,x,step_charac_Undershoot,x,step_charac_Peak,x,step_charac_PeakTime);

%     fig = figure;
%     hold on 
%     plot(x,step_charac_RiseTime,'bx',x,sti.RiseTime,'ro')
%     title('step charac RiseTime')
%     hold off
% 
%     fig = figure;
%     hold on 
%     plot(x,step_charac_SettlingTime,'bx',x,sti.SettlingTime,'ro')
%     title('step charac SettlingTime')
%     hold off
% 
%     fig = figure;
%     hold on 
%     plot(x,step_charac_SettlingMin,'bx',x,sti.RiseTime,'ro')
%     title('step charac SettlingMin')
%     hold off
% 
%     fig = figure;
%     hold on 
%     plot(x,step_charac_SettlingMax,'bx',x,sti.SettlingMax,'ro')
%     title('step charac SettlingMax')
%     hold off
% 
%     fig = figure;
%     hold on 
%     plot(x,step_charac_Overshoot,'bx',x,sti.Overshoot,'ro')
%     title('step charac Overshoot')
%     hold off
% 
%     fig = figure;
%     hold on 
%     plot(x,step_charac_Undershoot,'bx',x,sti.Undershoot,'ro')
%     title('step charac Undershoot')
%     hold off
% 
%     fig = figure;
%     hold on 
%     plot(x,step_charac_Peak,'bx',x,sti.Peak,'ro')
%     title('step charac Peak')
%     hold off
% 
%     fig = figure;
%     hold on 
%     plot(x,step_charac_PeakTime,'bx',x,sti.PeakTime,'ro')
%     title('step charac PeakTime')
%     hold off

end


mutant_templates(44)=44;
        
step_charac_RiseTime(44,overall_index)= base_sti.RiseTime;
step_charac_SettlingTime(44,overall_index)= base_sti.SettlingTime;
step_charac_SettlingMin(44,overall_index)= base_sti.SettlingMin;
step_charac_SettlingMax(44,overall_index)= base_sti.SettlingMax;
step_charac_Overshoot(44,overall_index)= base_sti.Overshoot;
step_charac_Undershoot(44,overall_index)= base_sti.Undershoot;
step_charac_Peak(44,overall_index)= base_sti.Peak;
step_charac_PeakTime(44,overall_index)= base_sti.PeakTime;

disp 'base_sti.RiseTime'
base_sti.RiseTime
unique(step_charac_RiseTime)
disp 'base_sti.SettlingTime'
base_sti.SettlingTime
unique(step_charac_SettlingTime)
disp 'base_sti.SettlingMin'
base_sti.SettlingMin
unique(step_charac_SettlingMin)
disp 'base_sti.SettlingMax'
base_sti.SettlingMax
unique(step_charac_SettlingMax)
disp 'base_sti.Overshoot'
base_sti.Overshoot
unique(step_charac_Overshoot)
disp 'base_sti.Undershoot'
base_sti.Undershoot
unique(step_charac_Undershoot)
disp 'base_sti.Peak'
base_sti.Peak
unique(step_charac_Peak)
disp 'base_sti.PeakTime'
base_sti.PeakTime
unique(step_charac_PeakTime)

fig_RiseTime=figure(1);
if ~exist('ExecueONce_1','var')
    set(fig_RiseTime,'Name','RiseTime parameter','NumberTitle','off')
    ExecueONce_1=0;
end
h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
hold on
plot(step_charac_RiseTime,'*')
tit=strcat(num2str(threshold_value), '% threshold');
title(h1,tit);
ylabel(h1,'# mutants');
xlabel(h1,'Mutant templates');
xTick=get(h1,'xtick');
set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
hold off

fig_SettlingTime=figure(2);
if ~exist('ExecueONce_2','var')
    set(fig_SettlingTime,'Name','SettlingTime parameter','NumberTitle','off')
    ExecueONce_2=0;
end
h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
hold on
plot(step_charac_SettlingTime,'*')
tit=strcat(num2str(threshold_value), '% threshold');
title(h1,tit);
ylabel(h1,'# mutants');
xlabel(h1,'Mutant templates');
xTick=get(h1,'xtick');
set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
hold off

fig_SettlingMin=figure(3);
if ~exist('ExecueONce_3','var')
    set(fig_SettlingMin,'Name','SettlingMin parameter','NumberTitle','off')
    ExecueONce_3=0;
end
h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
hold on
plot(step_charac_SettlingMin,'*')
tit=strcat(num2str(threshold_value), '% threshold');
title(h1,tit);
ylabel(h1,'# mutants');
xlabel(h1,'Mutant templates');
xTick=get(h1,'xtick');
set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
hold off

fig_SettlingMax=figure(4);
if ~exist('ExecueONce_4','var')
    set(fig_SettlingMax,'Name','SettlingMax parameter','NumberTitle','off')
    ExecueONce_4=0;
end
h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
hold on 
plot(step_charac_SettlingMax,'*')
% boxplot(step_charac_SettlingMax,mutant_templates)
tit=strcat(num2str(threshold_value), '% threshold');
title(h1,tit);
ylabel(h1,'# mutants');
xlabel(h1,'Mutant templates');
xTick=get(h1,'xtick');
set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
hold off

fig_Overshoot=figure(5);
if ~exist('ExecueONce_5','var')
    set(fig_Overshoot,'Name','Overshoot parameter','NumberTitle','off')
    ExecueONce_5=0;
end
h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
hold on 
plot(step_charac_Overshoot,'*')
tit=strcat(num2str(threshold_value), '% threshold');
title(h1,tit);
ylabel(h1,'# mutants');
xlabel(h1,'Mutant templates');
xTick=get(h1,'xtick');
set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
hold off

fig_Undershoot=figure(6);
if ~exist('ExecueONce_6','var')
    set(fig_Undershoot,'Name','Undershoot parameter','NumberTitle','off')
    ExecueONce_6=0;
end
h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
hold on
plot(step_charac_Undershoot,'*')
tit=strcat(num2str(threshold_value), '% threshold');
title(h1,tit);
ylabel(h1,'# mutants');
xlabel(h1,'Mutant templates');
xTick=get(h1,'xtick');
set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
hold off

fig_Peak=figure(7);
if ~exist('ExecueONce_7','var')
    set(fig_Peak,'Name','Peak parameter','NumberTitle','off')
    ExecueONce_7=0;
end
h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
hold on
plot(step_charac_Peak,'*')
tit=strcat(num2str(threshold_value), '% threshold');
title(h1,tit);
ylabel(h1,'# mutants');
xlabel(h1,'Mutant templates');
xTick=get(h1,'xtick');
set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
hold off

fig_PeakTime=figure(8);
if ~exist('ExecueONce_8','var')
    set(fig_PeakTime,'Name','PeakTime parameter','NumberTitle','off')
    ExecueONce_8=0;
end
h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
hold on
plot(step_charac_PeakTime,'*')
tit=strcat(num2str(threshold_value), '% threshold');
title(h1,tit);
ylabel(h1,'# mutants');
xlabel(h1,'Mutant templates');
xTick=get(h1,'xtick');
set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
hold off


% fig_RiseTime=figure(1);
% if ~exist('ExecueONce_1','var')
%     set(fig_RiseTime,'Name','RiseTime parameter','NumberTitle','off')
%     ExecueONce_1=0;
% end
% h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants_param(1,:)','-*b',xin',dead_mutants_param(1,:)','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% title(h1,tit);
% ylabel(h1,'# mutants');
% xlabel(h1,'Mutant templates');
% xTick=get(h1,'xtick');
% set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off
% 
% fig_SettlingTime=figure(2);
% if ~exist('ExecueONce_2','var')
%     set(fig_SettlingTime,'Name','SettlingTime parameter','NumberTitle','off')
%     ExecueONce_2=0;
% end
% h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants_param(2,:)','-*b',xin',dead_mutants_param(2,:)','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% title(h1,tit);
% ylabel(h1,'# mutants');
% xlabel(h1,'Mutant templates');
% xTick=get(h1,'xtick');
% set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off
% 
% fig_SettlingMin=figure(3);
% if ~exist('ExecueONce_3','var')
%     set(fig_SettlingMin,'Name','SettlingMin parameter','NumberTitle','off')
%     ExecueONce_3=0;
% end
% h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants_param(3,:)','-*b',xin',dead_mutants_param(3,:)','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% title(h1,tit);
% ylabel(h1,'# mutants');
% xlabel(h1,'Mutant templates');
% xTick=get(h1,'xtick');
% set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off
% 
% fig_SettlingMax=figure(4);
% if ~exist('ExecueONce_4','var')
%     set(fig_SettlingMax,'Name','SettlingMax parameter','NumberTitle','off')
%     ExecueONce_4=0;
% end
% h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants_param(4,:)','-*b',xin',dead_mutants_param(4,:)','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% title(h1,tit);
% ylabel(h1,'# mutants');
% xlabel(h1,'Mutant templates');
% xTick=get(h1,'xtick');
% set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off
% 
% fig_Overshoot=figure(5);
% if ~exist('ExecueONce_5','var')
%     set(fig_Overshoot,'Name','Overshoot parameter','NumberTitle','off')
%     ExecueONce_5=0;
% end
% h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants_param(5,:)','-*b',xin',dead_mutants_param(5,:)','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% title(h1,tit);
% ylabel(h1,'# mutants');
% xlabel(h1,'Mutant templates');
% xTick=get(h1,'xtick');
% set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off
% 
% 
% 
% fig_Undershoot=figure(6);
% if ~exist('ExecueONce_6','var')
%     set(fig_Undershoot,'Name','Undershoot parameter','NumberTitle','off')
%     ExecueONce_6=0;
% end
% h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants_param(6,:)','-*b',xin',dead_mutants_param(6,:)','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% title(h1,tit);
% ylabel(h1,'# mutants');
% xlabel(h1,'Mutant templates');
% xTick=get(h1,'xtick');
% set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off
% 
% fig_Peak=figure(7);
% if ~exist('ExecueONce_7','var')
%     set(fig_Peak,'Name','Peak parameter','NumberTitle','off')
%     ExecueONce_7=0;
% end
% h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants_param(7,:)','-*b',xin',dead_mutants_param(7,:)','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% title(h1,tit);
% ylabel(h1,'# mutants');
% xlabel(h1,'Mutant templates');
% xTick=get(h1,'xtick');
% set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off
% 
% fig_PeakTime=figure(8);
% if ~exist('ExecueONce_8','var')
%     set(fig_PeakTime,'Name','PeakTime parameter','NumberTitle','off')
%     ExecueONce_8=0;
% end
% h1 = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants_param(8,:)','-*b',xin',dead_mutants_param(8,:)','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% title(h1,tit);
% ylabel(h1,'# mutants');
% xlabel(h1,'Mutant templates');
% xTick=get(h1,'xtick');
% set(h1,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off
% 
% 
% fig_t=figure(9);
% if ~exist('ExecueONce_t','var')
%     set(fig_t,'Name','All parameter','NumberTitle','off')
%     ExecueONce_t=0;
% end
% h = subplot(fig_row,ceil(even_space_multiples/fig_row),fig_index);
% hold on
% plot(xin',all_mutants','-*r',xin',live_mutants','-*b',xin',dead_mutants','-*g')
% legend('all','live','dead')
% tit=strcat(num2str(threshold_value), '% threshold');
% title(h,tit);
% ylabel(h,'# mutants');
% xlabel(h,'Mutant templates');
% xTick=get(h,'xtick');
% set(h,'xTick',xin,'xticklabel',xlabel_dataout)
% hold off


fig_index = fig_index+1;
total_mutants

end


fclose('all')