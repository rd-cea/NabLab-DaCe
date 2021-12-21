/*******************************************************************************
 * Copyright (c) 2021 CEA
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 * Contributors: see AUTHORS file
 *******************************************************************************/
import { ApolloClient, ApolloProvider, DefaultOptions, HttpLink, InMemoryCache, split } from '@apollo/client';
import { WebSocketLink } from '@apollo/client/link/ws';
import { getMainDefinition } from '@apollo/client/utilities';
import { ServerContext } from '@eclipse-sirius/sirius-components';
import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import { JobsGraph } from './JobsGraph';

declare global {
    interface Window {
      acquireVsCodeApi(): any;
      projectName: string;
      nablaModelPath: string;
    }
}

const siriusWebServerAddress = 'http://127.0.0.1:8080';
// VSCode needs 127.0.0.1 instead of localhost
const value = { httpOrigin: siriusWebServerAddress };

const httpLink = new HttpLink({
  uri: `${siriusWebServerAddress}/api/graphql`,
  credentials: 'include',
});

const protocol = siriusWebServerAddress.startsWith('https') ? 'wss' : 'ws';
const address = siriusWebServerAddress.substring(siriusWebServerAddress.indexOf('://'), siriusWebServerAddress.length);
const subscriptionURL = `${protocol}${address}/subscriptions`;

const wsLink = new WebSocketLink({
  uri: subscriptionURL,
  options: {
    reconnect: true,
    lazy: true,
  },
});

const splitLink = split(
  ({ query }) => {
    const definition = getMainDefinition(query);
    return definition.kind === 'OperationDefinition' && definition.operation === 'subscription';
  },
  wsLink,
  httpLink
);

const defaultOptions: DefaultOptions = {
  watchQuery: {
    fetchPolicy: 'no-cache',
  },
  query: {
    fetchPolicy: 'no-cache',
  },
  mutate: {
    fetchPolicy: 'no-cache',
  },
};

const ApolloGraphQLClient = new ApolloClient({
  link: splitLink,
  cache: new InMemoryCache({ addTypename: true }),
  defaultOptions,
});
//http://localhost:3000/projects/f286e76a-05e6-4e43-a85a-179d5c7d7d72/edit/0b4045e2-0995-469f-94ce-12f43dd7d7d3
ReactDOM.render(
  <ServerContext.Provider value={value}>
    <ApolloProvider client={ApolloGraphQLClient}>
      <JobsGraph
        editingContextId={'f286e76a-05e6-4e43-a85a-179d5c7d7d72'}
        representationId={'0b4045e2-0995-469f-94ce-12f43dd7d7d3'}
        representationLabel={'ExplicitHeatEquation'}
        representationKind={'Diagram'}
      />
    </ApolloProvider>
  </ServerContext.Provider>,
  document.getElementById('root')
);
