within Buildings.ThermalZones.EnergyPlus.BaseClasses;
function initialize "Initialization"
  input Buildings.ThermalZones.EnergyPlus.BaseClasses.FMUZoneClass
    adapter "External object";
  input Real tStart "Simulation start time";
  output Modelica.SIunits.Area AFlo "Zone floor area";
  output Modelica.SIunits.Volume V "Zone air volume";
  output Real mSenFac "Factor for scaling the sensible thermal mass of the zone air volume";

  external "C" FMUZoneInitialize(adapter, tStart, AFlo, V, mSenFac)
  annotation (
  Include={"#include <FMUZoneInitialize.c>", "#include <EPFMI.hpp>"},
                   IncludeDirectory={"modelica://Buildings/Resources/C-Sources",
                   "modelica://Buildings/Resources/Include"});
  annotation (Documentation(info="<html>
<p>
External function to obtain parameters from the EnergyPlus FMU.
</p>
</html>", revisions="<html>
<ul><li>
March 1, 2018, by Michael Wetter:<br/>
First implementation.
</li>
</ul>
</html>"));

end initialize;
