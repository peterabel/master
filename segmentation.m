%% Small Script to plot and check the statistics of the samples
clc; clear(); close all;

%folder
folder = 'basketball';
featureAmount = 72;

for frame=1:1
    
    %Loading all Samples of all channels
    negSamples = importdata([folder '\frame' num2str(frame) '\negSamples.txt']);
    posSamples = importdata([folder '\frame' num2str(frame)  '\posSamples.txt']);
    negMuSigSq = importdata([folder '\frame' num2str(frame) '\negMuSigSq.txt']);
    posMuSigSq = importdata([folder '\frame' num2str(frame)  '\posMuSigSq.txt']);
    origImg = imread([folder '\groundTruth' num2str(frame)  '.png']);
    sampPositions = importdata([folder '\frame' num2str(frame) '\samplePositions.txt']);
    
    
    %Positive Mean and variance
    meanValuesPositive = posMuSigSq(:,1);
    varValuesPositive = posMuSigSq(:,2);
    meanValuesNegative = negMuSigSq(:,1);
    varValuesNegative = negMuSigSq(:,2);
    
    %Negative mean and variance
    mu_pos = meanValuesPositive;
    sigma_pos = sqrt(varValuesPositive);
    mu_neg = meanValuesNegative;
    sigma_neg = sqrt(varValuesNegative);
    
    ftr = 37;
    ftr2 = 1;
    figure;
    for channel = 1:10;
        stopAt = channel*featureAmount;
        beginAt = stopAt - featureAmount + 1;
        
        posMuSigSq_all = posMuSigSq(beginAt:stopAt,:);
        negMuSigSq_all = negMuSigSq(beginAt:stopAt,:);
        
        
        subplot(5,2,channel);
        plot(negMuSigSq_all(:,1),negMuSigSq_all(:,2),'.');
        hold on 
        plot(posMuSigSq_all(:,1),posMuSigSq_all(:,2),'.');
        hold on
        plot(posMuSigSq_all(ftr,1),posMuSigSq_all(ftr,2),'x');
        hold on
        plot(posMuSigSq_all(ftr2,1),posMuSigSq_all(ftr2,2),'x');
        grid;
        title(['Feature Space Channel' num2str(channel)]);
        xlabel('Mu');
        ylabel('sigma^2');
        legend('negative ftr','positive ftr', ['ftr' num2str(ftr)],['ftr' num2str(ftr2)]);
    end
    
    figure;
    imshow(origImg)
    x = sampPositions(ftr,1);
    y = sampPositions(ftr,2);
    w = sampPositions(ftr,3);
    h = sampPositions(ftr,4);
    rectangle('Position',[x,y,w,h],'EdgeColor','r');
    x = sampPositions(ftr2,1);
    y = sampPositions(ftr2,2);
    w = sampPositions(ftr2,3);
    h = sampPositions(ftr2,4);
    rectangle('Position',[x,y,w,h],'EdgeColor','b');

end