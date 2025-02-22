%% Small Script to plot and check the statistics of the samples
clc; clear(); close all;

%folder
folder = 'birds1';

for frame=1:1
    
    %Loading all Samples of all channels
    negSamples = importdata([folder '\frame' num2str(frame) '\negSamples.txt']);
    posSamples = importdata([folder '\frame' num2str(frame)  '\posSamples.txt']);
    negMuSigSq = importdata([folder '\frame' num2str(frame) '\negMuSigSq.txt']);
    posMuSigSq = importdata([folder '\frame' num2str(frame)  '\posMuSigSq.txt']);
    origImg = imread([folder '\groundTruth' num2str(frame)  '.png']);
    sampPositions = importdata([folder '\frame' num2str(frame) '\samplePositions.txt']);
    
    %Norming samples
    channelNum = 10;
    featPerChannel = size(posSamples,1)/channelNum;
    
    %Positive Mean and variance
    meanValuesPositive = posMuSigSq(:,1);%mean(posSamples,2);
    varValuesPositive = posMuSigSq(:,2);%var(posSamples,0,2);
    meanValuesNegative = negMuSigSq(:,1);%mean(posSamples,2);
    varValuesNegative = negMuSigSq(:,2);
    
    %Negative mean and variance
    mu_pos = meanValuesPositive;
    sigma_pos = sqrt(varValuesPositive);
    mu_neg = meanValuesNegative;
    sigma_neg = sqrt(varValuesNegative);
    
    indi = zeros(36,1);
    
    for feature = 1:36
        
        thresh = zeros(36,2);
        indicator = zeros(10,1);
        cnt = 1;
%         figure;
        for r = feature:36:360%:36
            
            y = linspace(min(min(posSamples(r,:)),min(negSamples(r,:))),max(max(posSamples(r,:)),max(negSamples(r,:))),1000);
            posGauss = exp(-(y-mu_pos(r)).^2./(2*sigma_pos(r)^2))./(sigma_pos(r)*sqrt(2*pi));
            negGauss = exp(-(y-mu_neg(r)).^2./(2*sigma_neg(r)^2))./(sigma_neg(r)*sqrt(2*pi));
            %
            
%             subplot(5,2,cnt);
%             h1 = histogram(posSamples(r,:),'Normalization','pdf');%,'binwidth',500);
%             title(['Histogram of channel ' num2str(cnt)]);
%             hold on
%             plot(y,posGauss,'LineWidth',1.5)
%             h2 = histogram(negSamples(r,:),'Normalization','pdf','binwidth',h1.get('binwidth'));%,'binwidth',500);
%             plot(y,negGauss,'LineWidth',1.5)
%             grid;
%             legend(['feat' num2str(feature) ' hist'],'gauss approx','bckgrnd hist','bckgrnd approx');
%             hold off
            
            thresh(r,1) = mu_neg(r)-sigma_neg(r);
            thresh(r,2) = mu_neg(r)+sigma_neg(r);
            indicator(cnt,1) = (mu_pos(r)<=mu_neg(r)-1.6*sigma_neg(r)) || (mu_pos(r)>=mu_neg(r)+1.6*sigma_neg(r) );
            %         disp(['sigma intervall: ' num2str(thresh(r,1)) ' - ' num2str(thresh(r,2))]);
            %         disp(['mean positive  : ' num2str(mu_pos(r))]);
            
            %         k=waitforbuttonpress;
            cnt = cnt+1;
        end
        indi(feature) = sum(indicator);
%             figure;
%             imshow(origImg)
%             x = sampPositions(feature,1);
%             y = sampPositions(r,2);
%             w = sampPositions(r,3);
%             h = sampPositions(r,4);
%             rectangle('Position',[x,y,w,h],'EdgeColor','r');
    end
    indi = indi/max(indi);
    
    results = zeros(size(origImg,1)+2,size(origImg,2)+2);
    for f=1:36
        x = sampPositions(f,1)+1;
        y = sampPositions(f,2)+1;
        w = sampPositions(f,3);
        h = sampPositions(f,4);
        
        results(y:(y+h-1),x:(x+w-1)) = results(y:(y+h-1),x:(x+w-1))+indi(f);       
    end
    results = results/3;
    figure;
    subplot(1,2,1)
    imshow(results);
    title('Result of the segmentation');
    subplot(1,2,2);
    imshow(origImg);
    title('Frame');
    
    maske = zeros(36,1);
    channel = 1;   
    stopAt = channel*featPerChannel;
    beginAt = stopAt - featPerChannel + 1;
%     
    maske(3:4) = 1; maske(7:8) = 1; maske(11:12) = 1; maske(14:16) = 1; maske(18:26) = 1; maske(29:30) = 1; maske(33:34) = 1;
    posMuSigSq_all = posMuSigSq(beginAt:stopAt,:);
    posMuSigSq_true = posMuSigSq_all(maske == 1,:);
    posMuSigSq_false = posMuSigSq_all(maske == 0,:);
    
    figure
    plot(negMuSigSq(beginAt:stopAt,1),negMuSigSq(beginAt:stopAt,2),'.');
    hold all
    plot(posMuSigSq_true(:,1),posMuSigSq_true(:,2),'x');
    plot(posMuSigSq_false(:,1),posMuSigSq_false(:,2),'.');
    grid;
    title('Feature Space');
    xlabel('Mu');
    ylabel('sigma^2');
    legend('negative Features','true positive Features','false positive Features');
    
end

%     
%     figure;
%     imshow(origImg)
%     hold on
%     for m=1:36
%         x = sampPositions(m,1);
%         y = sampPositions(m,2);
%         w = sampPositions(m,3);
%         h = sampPositions(m,4);
%         if(thresh(m) <= mu_pos(m))
%             rectangle('Position',[x,y,w,h],'EdgeColor','r');
%         else
%             rectangle('Position',[x,y,w,h],'EdgeColor','b');
%         end
%     end
%     hold off

%     
%     figure
%     plot(negMuSigSq(:,1),negMuSigSq(:,2),'.');
%     hold all
%     plot(posMuSigSq(:,1),posMuSigSq(:,2),'.');
%     grid;
%     title('Feature Space');
%     xlabel('Mu');
%     ylabel('sigma^2');
%     legend('negative Features','positive Features');
%  

    

%     maske = zeros(36,1);
%     channel = 9;   
%     stopAt = channel*featPerChannel;
%     beginAt = stopAt - featPerChannel + 1;
% %     
%     maske(3:4) = 1; maske(7:8) = 1; maske(11:12) = 1; maske(14:16) = 1; maske(18:26) = 1; maske(29:30) = 1; maske(33:34) = 1;
%     posMuSigSq_all = posMuSigSq(beginAt:stopAt,:);
%     posMuSigSq_true = posMuSigSq_all(maske == 1,:);
%     posMuSigSq_false = posMuSigSq_all(maske == 0,:);
%     
%     figure
%     plot(negMuSigSq(beginAt:stopAt,1),negMuSigSq(beginAt:stopAt,2),'.');
%     hold all
%     plot(posMuSigSq_true(:,1),posMuSigSq_true(:,2),'x');
%     plot(posMuSigSq_false(:,1),posMuSigSq_false(:,2),'.');
%     grid;
%     title('Feature Space');
%     xlabel('Mu');
%     ylabel('sigma^2');
%     legend('negative Features','true positive Features','false positive Features');