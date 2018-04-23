within Buildings.ThermalZones.EnergyPlus;
package BaseClasses "Package with base classes for Buildings.ThermalZones.EnergyPlus"
 extends Modelica.Icons.BasesPackage;


  function detectOperatingSystem
    "Detect the operating system and return 0 for Windows 64, 
  1 for Windows 32, 2 for Linux 64, and 3 for Linux32"
    output Integer retVal "Return value of the function";
    external "C";
    annotation (Include=
          "#include <detectOperatingSystem.c>", IncludeDirectory=
          "modelica://Buildings/Resources/C-Sources");

  end detectOperatingSystem;

  annotation (preferredView="info",
  Documentation(info="<html>
<p>
This package contains base classes that are used to construct the models in
<a href=\"modelica://Buildings.ThermalZones.EnergyPlus\">Buildings.ThermalZones.EnergyPlus</a>.
</p>
</html>"));
end BaseClasses;
