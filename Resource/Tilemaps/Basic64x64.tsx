<?xml version="1.0" encoding="UTF-8"?>
<tileset name="Basic64x64" tilewidth="64" tileheight="64" tilecount="100" columns="10">
 <image source="../textures/tilesheet01_64x64.bmp" trans="ff00ff" width="640" height="640"/>
 <tile id="18" type="PIPE_ENTRANCE"/>
 <tile id="19" type="ENEMY_SPAWN"/>
 <tile id="29" type="ENEMY_SPAWN">
  <properties>
   <property name="ENEMY_ID" type="int" value="0"/>
  </properties>
 </tile>
 <tile id="32">
  <animation>
   <frame tileid="32" duration="24"/>
   <frame tileid="27" duration="8"/>
   <frame tileid="28" duration="8"/>
   <frame tileid="27" duration="8"/>
  </animation>
 </tile>
 <tile id="65">
  <animation>
   <frame tileid="56" duration="100"/>
   <frame tileid="56" duration="100"/>
   <frame tileid="65" duration="100"/>
   <frame tileid="65" duration="100"/>
   <frame tileid="64" duration="100"/>
   <frame tileid="64" duration="100"/>
  </animation>
 </tile>
 <tile id="66">
  <animation>
   <frame tileid="66" duration="100"/>
   <frame tileid="63" duration="100"/>
   <frame tileid="63" duration="100"/>
   <frame tileid="63" duration="100"/>
   <frame tileid="63" duration="100"/>
   <frame tileid="63" duration="100"/>
  </animation>
 </tile>
</tileset>
