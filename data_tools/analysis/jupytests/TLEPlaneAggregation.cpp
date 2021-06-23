std::vector< std::vector < Satellite> >
	GroupByOrbitalPlanes (std::vector < Satellite> )
{
	double rtol_ref = 1; // [deg]

	std::vector< std::vector < Satellite> >  planes = { {m_constellationSats[0]} };
	uint3d_t i = 0;
	for (auto sat1: m_constellationSats)
	{

		if (i == 0)
		{
			continue;
		}
	
		// sat1.GetPosition();

			inc_sat1 = sat1.GetInclination();
			raan_sat1 = sat1.GetRAAN();
	
	// search for same raan-inc in each of the stored planes, 
		for (uint32_t planeid; planeid <= planes.size(); planeid++)
		{
		
			if planeid == planes.size()
			{
				planes.push_back( std::vector<Satellite> {sat1} )
				break;
			}
			Satellite sat2 = planes[planeidx][0];
			incSat2 = sat2.GetInclination();
			raanSat2 = sat2.GetRAAN();
		
			if ( (abs(incSat1 - incSat2) < rtol_ref ) &&  
				(abs(raanSat1 - raanSat2) < rtol_ref ) )
			{
				plane[planeidx].push_back(sat1);
				break;
			}
		}

		i ++;
	}
}