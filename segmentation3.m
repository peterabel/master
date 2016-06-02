%% Small Script to plot and check the statistics of the samples
clc; clear(); close all hidden;

% Initial settings
folder = 'birds1';
featureAmount = 72;
flag.plotChns = 0;
flag.plotGt = 0;

for frame=1
    %% Load Images etc
    if isunix == 1
        addpath(genpath('/home/peter/Dokumente/KIT/Masterarbeit/master/matlab/icf toolbox/toolbox'))
        frame1 = imread([folder '/00000001.jpg']);
        groundTruth = importfile([folder '/groundtruth.txt']);
        negSamples = importdata([folder '/frame' num2str(frame) '/negSamples.txt']);
        posSamples = importdata([folder '/frame' num2str(frame)  '/posSamples.txt']);
        negMuSigSq = importdata([folder '/frame' num2str(frame) '/negMuSigSq.txt']);
        posMuSigSq = importdata([folder '/frame' num2str(frame)  '/posMuSigSq.txt']);
        origImg = imread([folder '/groundTruth' num2str(frame)  '.png']);
        if frame==1
            sampPositions = importdata([folder '/frame' num2str(frame) '/samplePositions.txt']);
        end
    else
        frame1 = imread([folder '\00000001.jpg']);
        groundTruth = importfile([folder '\groundtruth.txt']);
        negSamples = importdata([folder '\frame' num2str(frame) '\negSamples.txt']);
        posSamples = importdata([folder '\frame' num2str(frame)  '\posSamples.txt']);
        negMuSigSq = importdata([folder '\frame' num2str(frame) '\negMuSigSq.txt']);
        posMuSigSq = importdata([folder '\frame' num2str(frame)  '\posMuSigSq.txt']);
        origImg = imread([folder '\groundTruth' num2str(frame)  '.png']);
        sampPositions = importdata([folder '\frame' num2str(frame) '\samplePositions.txt']);
    end
    
    %% Plotting Image and Feature Channels
    if flag.plotGt == 1
        figure;
        imshow(frame1);
        v = [groundTruth(1,1:2); groundTruth(1,3:4); groundTruth(1,5:6); groundTruth(1,7:8)];
        patch('Faces',[1,2,3,4],'Vertices',v,'EdgeColor','blue',...
            'FaceColor','none','LineWidth',2);
        chns = chnsCompute(frame1);
    end
    if flag.plotChns == 1
        figure
        subplot(5,2,1);
        imshow(chns.data{1,1}(:,:,1));
        title('Channel 1');
        subplot(5,2,2);
        imshow(chns.data{1,1}(:,:,2));
        title('Channel 2');
        subplot(5,2,3);
        imshow(chns.data{1,1}(:,:,3));
        title('Channel 3');
        subplot(5,2,4);
        imshow(chns.data{1,2}(:,:));
        title('Channel 4');
        subplot(5,2,5);
        imshow(chns.data{1,3}(:,:,1));
        title('Channel 5');
        subplot(5,2,6);
        imshow(chns.data{1,3}(:,:,2));
        title('Channel 6');
        subplot(5,2,7);
        imshow(chns.data{1,3}(:,:,3));
        title('Channel 7');
        subplot(5,2,8);
        imshow(chns.data{1,3}(:,:,4));
        title('Channel 8');
        subplot(5,2,9);
        imshow(chns.data{1,3}(:,:,5));
        title('Channel 9');
        subplot(5,2,10);
        imshow(chns.data{1,3}(:,:,6));
        title('Channel 10');
    end
    
    %% Generating Features
    
    %Basketball intervall 17 - 56
    
    numBin = 40;
    
    indeces = zeros(720,1);
    probs = zeros(720,200);
    probs2 = zeros(720,2);
    D = zeros(10,1);%numBin*(numBin-1)*0.5);
    stats = zeros(10,4);
    
    chnnl = 1;
    cnt = 1;
%     fg1 = figure;
        for r=1:72:720
            
            %for basketball
            if strcmp(folder, 'basketball')
                surrInv = [r, r+15;r+56, r+71];
                boxInv = [r+16, r+55];
                surroundFtr = [posSamples(surrInv(1,1):surrInv(1,2),samp)
                    posSamples(surrInv(2,1):surrInv(2,2),samp)];
                boxFtr = posSamples(boxInv(1):boxInv(2),samp);
            end
            %for birds1
            if strcmp(folder,'birds1')
                surrInv = r+[1:3,7:11,15:19,23:27,31:35,39:43,47:51,55:59,63:71]-1;
                boxInv = r+[4,5,6,12,13,14,20,21,22,28,29,30,36,37,38,44,45,46,52,53,54,60,61,62]-1;
                surroundFtr = posSamples(surrInv,:);
                boxFtr = posSamples(boxInv,:);
            end
            
            % Gauss Approximation for decision
            mu_1 = mean(reshape(boxFtr,[1 size(boxFtr,1)*size(boxFtr,2)]));
            sigma_1 = sqrt(var(reshape(boxFtr,[1 size(boxFtr,1)*size(boxFtr,2)])));
            mu_0 = mean( reshape(surroundFtr,[1 size(surroundFtr,1)*size(surroundFtr,2)]));
            sigma_0 = sqrt(var( reshape(surroundFtr,[1 size(surroundFtr,1)*size(surroundFtr,2)])));
            
            
            stats(chnnl,1) = mu_0;
            stats(chnnl,2) = sigma_0;
            stats(chnnl,3) = mu_1;
            stats(chnnl,4) = sigma_1;
            stats(chnnl,5) = sigma_1/sigma_0;
            
            y = linspace(min(min([surroundFtr; boxFtr])),max(max([surroundFtr; boxFtr])),1000);
            y_steps = (max(max([surroundFtr; boxFtr]))-min(min([surroundFtr; boxFtr])))/1000;
            posGauss = exp(-(y-mu_1).^2./(2*sigma_1^2))./(sigma_1*sqrt(2*pi));
            negGauss = exp(-(y-mu_0).^2./(2*sigma_0^2))./(sigma_0*sqrt(2*pi));
            
            binEdges = linspace(min(min([surroundFtr;boxFtr])),max(max([surroundFtr;boxFtr])),numBin+1);
            
            [f_1, xi_1] = ksdensity(reshape(boxFtr,[1 size(boxFtr,1)*size(boxFtr,2)]),'Kernel','epanechnikov');
            [f_0, xi_0] = ksdensity(reshape(surroundFtr,[1 size(surroundFtr,1)*size(surroundFtr,2)]),'Kernel','epanechnikov');
            
%             figure(fg1);
%             subplot(5,2,chnnl);
%             h1 = histogram( surroundFtr ,'Normalization','probability','BinEdges', binEdges);%'NumBins',numBin);
%             title('Histogram in Channel1');
%             hold on
%             h2 = histogram(boxFtr,'Normalization','probability','BinEdges', binEdges);%'NumBins',numBin);
%             %             plot(y,negGauss,'LineWidth',2);
%             %             plot(y,posGauss,'LineWidth',2);
% %                     plot(xi_0,f_0,'LineWidth',2);
% %                     plot(xi_1,f_1,'LineWidth',2);
%             grid;
%             title(['Histograms in Channel ' num2str(chnnl)]);
%             xlabel('ftr value');
%             ylabel('pdf');
%             legend('surround ftr','box ftr');
        
        
        chnnl = chnnl+1;
    
    % Ordering all probabilities
    for m=r:(r+71)
%         %find index
%         for samp = 1:100
%             bin = find((h1.BinEdges > posSamples(m,samp)),1)-1;
% 
%             if isempty(bin)
%                 bin = numBin-1;
%             end
%             if bin == 0
%                 bin = 1;
%             end
            
%             factors = 0.5:0.25:2;
%             cnt = 1;
%             if posSamples(m,samp) < xi_1(1)
%                 index1 = 2;
%             elseif posSamples(m,samp) > xi_1(end)
%                 index1 = length(xi_1);
%             else
%                 index1 = find((abs(xi_1-posSamples(m,samp))<0.25*(xi_1(2)-xi_1(1)))== 1);
%             end
%             
%             while isempty(index1)
%                 index1 = find((abs(xi_1-posSamples(m,samp))<factors(cnt)*(xi_1(2)-xi_1(1)))== 1);
%                 cnt = cnt+1;
%             end
%             if size(index1,2)>1
%                 index1 = index1(1);
%             end
%             cnt = 1;
%             if posSamples(m,samp) < xi_0(1)
%                 index0 = 2;
%             elseif posSamples(m,samp) > xi_0(end)
%                 index0 = length(xi_0);
%             else
%                 index0 = find((abs(xi_0-posSamples(m,samp))<0.25*(xi_0(2)-xi_0(1)))== 1);
%             end
%             
%             while isempty(index0)
%                 index0 = find((abs(xi_0-posSamples(m,samp))<factors(cnt)*(xi_0(2)-xi_0(1)))== 1);
%                 cnt = cnt+1;
%             end
%             if size(index0,2)>1
%                 index0 = index0(1);
%             end
%             
%             probs(m,2*samp-1) = h2.Values(bin);%f_1(index1);
%             probs(m,2*samp) = h1.Values(bin);%f_0(index0);
        end
        end
        
        

    
end
    