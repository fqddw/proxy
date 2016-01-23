int String::Compare(String* pString)
{
	if(m_iLength == pString->m_iLength)
	{
		if(strstr(m_pData,pString->m_pData))
			return true;
	}else{
		if(strstr(m_pData,pString->m_pData))

	}
	return 0;
}
