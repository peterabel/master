%%% Generating ConfMap
clc;clear;close all;

addpath(genpath('G:\abel\confMap\outlier detection'));

%loading
filename = 'car1_prediction.txt';
rawData = load(filename);

pred = rawData(:,1);
[maxVal ind] = max(pred);

%find out image height
[diffVals diffInds] = sort(rawData(1:end-1,2)-rawData(2:end,2));
height = diffInds(1);

%total center point
centerPointX = rawData(ind,2)+0.5*rawData(ind,4);%rawData(1,6)+0.5*rawData(1,8);
centerPointY = rawData(ind,3)+0.5*rawData(ind,5);%rawData(1,7)+0.5*rawData(1,9);
%all center points
cpsX = rawData(:,2)+0.5*rawData(:,4);
cpsY = rawData(:,3)+0.5*rawData(:,5);
%euclidean differences to the middle
dist_x = sqrt((cpsX-centerPointX).^2);
dist_y = sqrt((cpsY-centerPointY).^2);
dist = sqrt((cpsX-centerPointX).^2+(cpsY-centerPointY).^2);

dist_norm = dist./max(dist);
fourthMax = sort(pred,'descend')
fourthMax = fourthMax(8);

%pred(find(pred>=fourthMax))=fourthMax;

pred_dist = pred./dist;

confMap = reshape(pred_dist,height,[]);

%confMap(find(confMap>=mean(mean(confMap))))=mean(mean(confMap));
se = strel('disk',2);
IM2 = imclose(confMap,se);

figure();
imagesc(confMap);
colorbar();
% %find highest llls
% numSel = 50;
% [lll_sorted,ind_sorted] = sort(lll,'descend');
% 
% % sortedX will be in descending order. Therefore, the first N elements will be the N maximum values.
% 
% selectedVal = lll_sorted(1:numSel);
% selectedInd = ind_sorted(1:numSel);
% 
% %plotting 
% figure();
% title('LogLikelihood versus distance');
% subplot(3,1,1);
% plot(dist(1:64),lll(1:64),'x');
% xlabel('distance');
% ylabel('logLikl');
% grid();
% subplot(3,1,2);
% plot(dist(65:128),lll(65:128),'x');
% xlabel('distance');
% ylabel('logLikl');
% grid();
% subplot(3,1,3);
% plot(dist(129:193),lll(129:193),'x');
% xlabel('distance');
% ylabel('logLikl');
% grid();
% 
% figure();
% plot(dist(1:640),lll(1:640),'o');
% hold on
% plot(dist(641:941),lll(641:941),'o');
% plot(dist(942:1100),lll(942:1100),'o');
% plot(dist(1101:1140),lll(1101:1140),'o');
% plot(dist(1141:1150),lll(1141:1150),'o');
% hold off;
% xlabel('distance');
% ylabel('logLikl');
% grid();
% 
% figure();
% plot3(dist_x(1:640),dist_y(1:640),lll(1:640),'o');
% hold on
% plot3(dist_x(641:941),dist_y(641:941),lll(641:941),'o');
% plot3(dist_x(942:1100),dist_y(942:1100),lll(942:1100),'o');
% plot3(dist_x(1101:1140),dist_y(1101:1140),lll(1101:1140),'o');
% plot3(dist_x(1141:1150),dist_y(1141:1150),lll(1141:1150),'o');
% hold off;
% xlabel('distance x');
% ylabel('distance y');
% zlabel('logLikl');
% grid();
