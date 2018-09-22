<?xml version="1.0" encoding="UTF-8"?>
<tileset name="MetaTiles" tilewidth="64" tileheight="64" tilecount="22" columns="11">
 <image source="../textures/MetaTiles.bmp" trans="ff00ff" width="704" height="128"/>
 <tile id="0" type="BLOCK"/>
 <tile id="1" type="ITEM"/>
 <tile id="2" type="ITEM">
  <properties>
   <property name="item" type="int" value="1"/>
  </properties>
 </tile>
 <tile id="3" type="ITEM">
  <properties>
   <property name="item" type="int" value="2"/>
  </properties>
 </tile>
 <tile id="4" type="WARP"/>
 <tile id="5" type="WARP_EXIT">
  <properties>
   <property name="exitdirection" type="int" value="0"/>
   <property name="locky" type="int" value="1"/>
  </properties>
 </tile>
 <tile id="10" type="ITEM">
  <properties>
   <property name="item" type="int" value="0"/>
  </properties>
 </tile>
 <tile id="11" type="ENEMY_SPAWN"/>
 <tile id="12" type="ENEMY_SPAWN">
  <properties>
   <property name="ENEMY_ID" type="int" value="1"/>
  </properties>
 </tile>
 <tile id="13" type="ITEM">
  <properties>
   <property name="item" type="int" value="3"/>
  </properties>
 </tile>
 <tile id="14" type="CONTROL"/>
 <tile id="15" type="ITEM">
  <properties>
   <property name="item" type="int" value="4"/>
  </properties>
 </tile>
 <tile id="18" type="FLAG_POLE"/>
</tileset>
