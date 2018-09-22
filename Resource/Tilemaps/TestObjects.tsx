<?xml version="1.0" encoding="UTF-8"?>
<tileset name="TestObjects" tilewidth="64" tileheight="64" tilecount="4" columns="0">
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="0">
  <image width="64" height="64" source="../textures/Goomba.bmp"/>
 </tile>
 <tile id="1" type="ITEM">
  <properties>
   <property name="item" type="int" value="2"/>
  </properties>
  <image width="64" height="64" source="../textures/1upBlock.bmp"/>
 </tile>
 <tile id="2" type="ITEM">
  <image width="64" height="64" source="../textures/CoinBlock.bmp"/>
 </tile>
 <tile id="3" type="ITEM">
  <properties>
   <property name="item" type="int" value="1"/>
  </properties>
  <image width="64" height="64" source="../textures/PowerupBlock.bmp"/>
 </tile>
</tileset>
