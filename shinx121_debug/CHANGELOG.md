## v0.4.0
- Added error prints for invalid attempts at starting an object sequence.
- Added debug window for inspecting the state of the ANIM DLL (ID 3).
- Added object editor tab for playing any model animation for the object's active model.
- Added inspector for KamerianBoss object data.
- Small updates to the object editor.

## v0.3.0
- Added object type list.
- Added debug window for blocks, hits, and mod lines.
- Added debug window for editing map acts and object setup groups.
- Added debug window for visualizing and inspecting curves.
- Added debug window for playing sound effects, music, and MPEG files directly.
- Added debug window for testing particles.
- Added debug window for inspecting race DLL state.
- Added debug window for viewing shadow buffer usage/capacity.
- Added debug window for viewing/editing time of day.
- Added debug window for triggering subtitles for any game text.
- Added inspector/editor for trigger setup and object data.
- Added inspector for KTrex object data.
- Added inspector for BWLog object data.
- Added inspector for IMSnowBike object data.
- Added inspector for SEQOBJ object data + a way to trigger their sequences directly.
- Object sequences can be played directly from their edit window.
- DLL 27 and objfsa data can be viewed for some objects.
- Triggers are now more accurately visualized in 3D.
- EffectBox objects are now visualized correctly in 3D.
- Alarms for some filesystem and object load related errors.
- Updated object properties from the decomp.
- Fix incorrect dbgui_end_child calls.
- Object setup crash fixes.

## v0.2.0
- Added button in the object editor to destroy the object.
- Object create info now includes a hex dump of the "additional" object specific create info data.
- Added Bit Table Debug window for modifying/viewing global game flags/state.
- Fixed crash that occurred when an object editor window was open for a freed object.

## v0.1.0
- Initial release.
